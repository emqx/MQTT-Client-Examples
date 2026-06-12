/**
 * @file main.c
 * @brief Zephyr MQTT 多功能 Shell 客户端
 *
 * 通过 Zephyr Shell 提供 mqtt_cli conn / sub / pub 三个子命令，支持：
 *   - NSOS/TCP-only 模式（prj-nsos.conf overlay）：明文 MQTT，无需证书
 *   - TAP+TLS 模式（prj-tap-tls.conf overlay）：TLS 1.2 + mTLS 双向认证
 *
 * 所有子命令共享 common_mqtt_connect() 作为连接引擎：
 *   DNS 解析 → mqtt_client_init → TLS 凭据装载（可选）→ mqtt_connect
 *   → 轮询等待 CONNACK → 打印 Connection successful!
 *
 * 事件回调 mqtt_evt_handler() 处理 CONNACK / DISCONNECT / PUBLISH，
 * 收到消息时通过全局 shell 指针使用 shell_print 输出（避免 native_sim
 * 上 LOG_INF / printk 的双通道输出重复问题）。
 */

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/sys_getopt.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* ── TLS 凭据管理（仅 TAP+TLS 模式编译） ─────────────────────────────── */

#ifdef CONFIG_MQTT_LIB_TLS
#include <zephyr/net/tls_credentials.h>

/** Zephyr 凭据安全标签，所有 CA/Cert/Key 均注入到 tag 101 */
static sec_tag_t sec_tag_list[] = { 101 };

/**
 * @brief 记录已注入凭据的类型和缓冲区指针，用于跨连接清理
 */
struct credential_slot {
    enum tls_credential_type type;   /**< 凭据类型（CA / 公钥证书 / 私钥） */
    uint8_t *buf;                    /**< malloc 分配的缓冲区指针 */
};

static struct credential_slot ca_credential = {
    .type = TLS_CREDENTIAL_CA_CERTIFICATE,
};
static struct credential_slot cert_credential = {
    .type = TLS_CREDENTIAL_PUBLIC_CERTIFICATE,
};
static struct credential_slot key_credential = {
    .type = TLS_CREDENTIAL_PRIVATE_KEY,
};
#endif

LOG_MODULE_REGISTER(mqtt_app, LOG_LEVEL_INF);

/* ── 全局状态 ────────────────────────────────────────────────────────── */

/**
 * @brief 全局 shell 指针，供事件回调使用 shell_print 输出消息。
 *
 * native_sim 平台上 printk / LOG_INF 会因 UART native PTY 和 log backend
 * 双通道同时输出到 stdout 导致每条日志打印两次。只有 shell_print 仅走
 * UART 单通道，输出不重复。
 */
static const struct shell *mqtt_evt_shell;

/** getopt 长选项枚举，避免 magic number */
enum mqtt_cli_opt {
    OPT_HELP = 1000,
    OPT_KEY,
    OPT_CERT,
    OPT_CA,
    OPT_INSECURE,
    OPT_KEY_PASSWORD,
    OPT_NO_CLEAN
};

/** MQTT 接收 / 发送缓冲区：全局分配，避免 shell 线程栈溢出 */
static uint8_t rx_buffer[1024];
static uint8_t tx_buffer[1024];

/** 全局唯一的 MQTT 客户端实例 */
static struct mqtt_client client_ctx;

/** 解析后的 broker 地址 */
static struct sockaddr_storage broker_addr;

/** 连接时使用的 Client ID */
static char client_id_global[32];

/** 连接状态标志：MQTT_EVT_CONNACK 后置 true，DISCONNECT 后置 false */
static volatile bool is_connected = false;

/* ── 数据结构 ────────────────────────────────────────────────────────── */

/**
 * @brief 连接参数结构体，统一承载 conn / sub / pub 的命令行参数
 */
struct mqtt_conn_params {
    char host[64];           /**< Broker 地址 */
    int port;                /**< Broker 端口 */
    char client_id[32];      /**< MQTT Client ID */
    int keepalive;           /**< 心跳间隔（秒） */
    char username[64];       /**< 用户名（可选） */
    char password[64];       /**< 密码（可选） */
    bool use_tls;            /**< 是否启用 TLS */
    char key_path[128];      /**< 客户端私钥路径 */
    char cert_path[128];     /**< 客户端证书路径 */
    char ca_path[128];       /**< CA 证书路径 */
    bool insecure;           /**< 是否跳过服务端证书验证 */
    char key_password[64];   /**< 私钥密码（若加密） */
    bool no_clean;           /**< 是否禁用 Clean Session */
};

/** getopt 长选项表，三个子命令共用 */
static const struct sys_getopt_option long_options[] = {
    {"topic",       1, NULL, 't'},
    {"message",     1, NULL, 'm'},
    {"qos",         1, NULL, 'q'},
    {"Interval",    1, NULL, 'I'},
    {"limit",       1, NULL, 'L'},
    {"retain",      0, NULL, 'r'},
    {"dup",         0, NULL, 'd'},
    {"client_id",   1, NULL, 'i'},
    {"host",        1, NULL, 'h'},
    {"port",        1, NULL, 'p'},
    {"keepalive",   1, NULL, 'k'},
    {"username",    1, NULL, 'u'},
    {"password",    1, NULL, 'P'},
    {"key",         1, NULL, OPT_KEY},
    {"cert",        1, NULL, OPT_CERT},
    {"ca",          1, NULL, OPT_CA},
    {"insecure",    0, NULL, OPT_INSECURE},
    {"key_password",1, NULL, OPT_KEY_PASSWORD},
    {"no_clean",    0, NULL, OPT_NO_CLEAN},
    {"help",        0, NULL, OPT_HELP},
    {0, 0, 0, 0}
};

/* ==================================================================== */
/* 帮助信息输出                                                        */
/* ==================================================================== */

/**
 * @brief 打印 conn / sub / pub 共享的通用参数帮助
 * @param sh Shell 实例
 */
static void print_common_options_help(const struct shell *sh)
{
    shell_print(sh, "Connection Options:");
    shell_print(sh, "  -h, --host <STRING>      Broker address (default: broker.emqx.io)");
    shell_print(sh, "  -p, --port <NUMBER>      Broker port (default: 1883)");
    shell_print(sh, "  -i, --client_id <STRING> MQTT Client ID (default: random)");
    shell_print(sh, "  -k, --keepalive <SEC>    Keepalive interval in seconds (default: 60)");
    shell_print(sh, "  -u, --username <USER>    Username for broker authentication");
    shell_print(sh, "  -P, --password <PASS>    Password for broker authentication");

#ifdef CONFIG_MQTT_LIB_TLS
    shell_print(sh, "TLS Options:");
    shell_print(sh, "  --ca <PATH>              Path to CA certificate file");
    shell_print(sh, "  --cert <PATH>            Path to client certificate file");
    shell_print(sh, "  --key <PATH>             Path to client private key file");
    shell_print(sh, "  --insecure               Skip server certificate verification (default: false)");
    shell_print(sh, "  --key_password <PASS>    Password for encrypted private key file");
    shell_print(sh, "  --no_clean               Disable clean session (default: clean session enabled)");
#endif
}

/**
 * @brief 打印 mqtt_cli conn 帮助
 */
static void print_conn_help(const struct shell *sh)
{
    shell_print(sh, "Usage: mqtt_cli conn [options]");
    print_common_options_help(sh);
}

/**
 * @brief 打印 mqtt_cli sub 帮助
 */
static void print_sub_help(const struct shell *sh)
{
    shell_print(sh, "Usage: mqtt_cli sub -t <TOPIC> [options]");
    shell_print(sh, "Options:");
    shell_print(sh, "  -t, --topic <STRING>     [Required] MQTT topic to subscribe to");
    shell_print(sh, "  -q, --qos <0|1|2>        QoS level (default: 0)");
    print_common_options_help(sh);
}

/**
 * @brief 打印 mqtt_cli pub 帮助
 */
static void print_pub_help(const struct shell *sh)
{
    shell_print(sh, "Usage: mqtt_cli pub -t <TOPIC> -m <MSG> [options]");
    shell_print(sh, "Options:");
    shell_print(sh, "  -t, --topic <STRING>     [Required] MQTT topic to publish to");
    shell_print(sh, "  -m, --message <STRING>   [Required] MQTT message payload");
    shell_print(sh, "  -q, --qos <0|1|2>        QoS level (default: 0)");
    shell_print(sh, "  -I, --Interval <NUMBER>  Interval between multiple publishes (in ms)");
    shell_print(sh, "  -L, --limit <NUMBER>     Total number of messages to publish (default: 1)");
    shell_print(sh, "  -r, --retain             Set Retain flag (default: false)");
    shell_print(sh, "  -d, --dup                Set Duplicate flag (default: false)");
    print_common_options_help(sh);
}

/* ==================================================================== */
/* TLS 凭据装载与管理（仅 CONFIG_MQTT_LIB_TLS=y 时编译）                 */
/* ==================================================================== */

#ifdef CONFIG_MQTT_LIB_TLS

/**
 * @brief 根据凭据类型返回对应的 credential_slot
 */
static struct credential_slot *credential_slot_for_type(enum tls_credential_type type)
{
    switch (type) {
    case TLS_CREDENTIAL_CA_CERTIFICATE:
        return &ca_credential;
    case TLS_CREDENTIAL_PUBLIC_CERTIFICATE:
        return &cert_credential;
    case TLS_CREDENTIAL_PRIVATE_KEY:
        return &key_credential;
    default:
        return NULL;
    }
}

/**
 * @brief 清除已注册的 TLS 凭据（删除 Zephyr 缓存 + 释放 malloc 缓冲）
 *
 * 每次连接前必须清理旧凭据，避免跨连接的证书残留导致 mTLS 握手失败
 * （-0x4e / -103）。
 */
static void clear_registered_credential(enum tls_credential_type type)
{
    struct credential_slot *slot = credential_slot_for_type(type);

    tls_credential_delete(101, type);

    if (slot && slot->buf) {
        free(slot->buf);
        slot->buf = NULL;
    }
}

/**
 * @brief 从文件加载证书并注入 Zephyr 凭据缓存（sec_tag 101）
 *
 * 为每类凭据 malloc 独立缓冲（不能用 static buffer，因后续调用会覆盖），
 * 末尾补 NUL（crt_is_pem() 需要），调用前先 clear 旧凭据。
 *
 * @param sh   Shell 实例
 * @param path 证书文件路径
 * @param type 凭据类型
 * @return 0 成功，负值为 errno
 */
static int load_and_register_credential(const struct shell *sh, const char *path, enum tls_credential_type type)
{
    struct credential_slot *slot = credential_slot_for_type(type);
    FILE *f = fopen(path, "rb");
    if (!f) {
        shell_error(sh, "Error: Cannot open credential file: %s", path);
        return -ENOENT;
    }

    /* Use malloc: each credential needs its own persistent buffer.
     * static buffer would be overwritten by subsequent calls (CA → cert → key). */
    uint8_t *file_buf = malloc(3073); /* +1 for null terminator (crt_is_pem requires it) */
    if (!file_buf) {
        fclose(f);
        shell_error(sh, "Error: Out of memory loading credential: %s", path);
        return -ENOMEM;
    }

    size_t br = fread(file_buf, 1, 3072, f);
    fclose(f);

    if (br == 0) {
        shell_error(sh, "Error: Credential file is empty: %s", path);
        free(file_buf);
        return -EIO;
    }

    file_buf[br] = '\0';  /* crt_is_pem() checks buf[len-1] == '\0' */
    clear_registered_credential(type);
    int rc = tls_credential_add(101, type, file_buf, br + 1);  /* include null */
    if (rc < 0) {
        shell_error(sh, "Error: Failed to inject TLS credential into Zephyr cache: %d", rc);
        free(file_buf);
        return rc;
    }

    if (slot) {
        slot->buf = file_buf;
    }

    return 0;
}
#endif

/* ==================================================================== */
/* MQTT 事件回调                                                        */
/* ==================================================================== */

/**
 * @brief Zephyr MQTT 库异步事件回调
 *
 * 处理 CONNACK（设置连接标志）、DISCONNECT（清除连接标志）、
 * PUBLISH（读取 payload 并通过 shell_print 输出）。
 * QoS 1 消息自动发送 PUBACK，QoS 2 消息发送 PUBREC。
 *
 * @param client MQTT 客户端实例
 * @param evt    事件描述
 */
void mqtt_evt_handler(struct mqtt_client *client, const struct mqtt_evt *evt)
{
    switch (evt->type) {
    case MQTT_EVT_CONNACK:
        if (evt->result == 0) {
            is_connected = true;
        } else {
            LOG_ERR("MQTT connection refused: %d", evt->result);
        }
        break;
    case MQTT_EVT_DISCONNECT:
        is_connected = false;
        break;
    case MQTT_EVT_PUBLISH: {
        const struct mqtt_publish_param *pub = &evt->param.publish;

        /* QoS 1: 必须先发送 PUBACK 确认，否则 broker 会不断重传 */
        if (pub->message.topic.qos == MQTT_QOS_1_AT_LEAST_ONCE)
        {
            const struct mqtt_puback_param ack_param = {
                .message_id = pub->message_id};
            mqtt_publish_qos1_ack(client, &ack_param);
        }
        else if (pub->message.topic.qos == MQTT_QOS_2_EXACTLY_ONCE)
        {
            const struct mqtt_pubrec_param rec_param = {
                .message_id = pub->message_id};
            mqtt_publish_qos2_receive(client, &rec_param);
        }

        uint8_t payload_buf[128];
        int len = MIN(pub->message.payload.len, sizeof(payload_buf) - 1);
        
        int rc = mqtt_read_publish_payload(client, payload_buf, len);
        if (rc >= 0) {
            payload_buf[rc] = '\0';
            if (mqtt_evt_shell) {
                shell_print(mqtt_evt_shell,
                    "[Received Msg] Topic: %.*s | Payload: %s",
                    pub->message.topic.topic.size,
                    pub->message.topic.topic.utf8, payload_buf);
            }
        }
        break;
    }
    default:
        break;
    }
}

/**
 * @brief 获取当前传输层的 socket 文件描述符（兼容 TCP / TLS 两种模式）
 */
static int get_client_fd(struct mqtt_client *client)
{
#ifdef CONFIG_MQTT_LIB_TLS
    if (client->transport.type == MQTT_TRANSPORT_SECURE) {
        return client->transport.tls.sock;
    }
#endif
    return client->transport.tcp.sock;
}

/* ==================================================================== */
/* 通用 MQTT 连接引擎                                                   */
/* ==================================================================== */

/**
 * @brief MQTT 连接公共入口，conn / sub / pub 三个命令共用
 *
 * 依次执行：
 *   1. 随机 Client ID（若未指定）
 *   2. DNS 解析（zsock_getaddrinfo）
 *   3. mqtt_client_init + 参数绑定
 *   4. TLS 凭据装载（若 use_tls）
 *   5. mqtt_connect 发送 CONNECT 报文
 *   6. 100ms 间隔轮询等待 CONNACK（超时 5 秒）
 *
 * @param sh Shell 实例
 * @param p  连接参数
 * @return 0 成功，负值为 errno
 */
static int common_mqtt_connect(const struct shell *sh, struct mqtt_conn_params *p)
{
    int rc;
    is_connected = false;

    mqtt_evt_shell = sh;  /* 记录 shell 指针供事件回调使用 */

    if (strlen(p->client_id) == 0) {
        snprintf(client_id_global, sizeof(client_id_global), "zephyr-emqx-%06u", sys_rand32_get() % 1000000);
    } else {
        strncpy(client_id_global, p->client_id, sizeof(client_id_global) - 1);
    }

    struct zsock_addrinfo hints = { .ai_family = AF_INET, .ai_socktype = SOCK_STREAM };
    struct zsock_addrinfo *res = NULL;
    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", p->port);

    rc = zsock_getaddrinfo(p->host, port_str, &hints, &res);
    if (rc != 0) {
        shell_error(sh, "Error: Failed to resolve host address %s:%d", p->host, p->port);
        return rc;
    }
    memcpy(&broker_addr, res->ai_addr, res->ai_addrlen);
    zsock_freeaddrinfo(res);

    mqtt_client_init(&client_ctx);
    client_ctx.clean_session = p->no_clean ? 0U : 1U;
    client_ctx.broker = &broker_addr;
    client_ctx.evt_cb = mqtt_evt_handler;
    client_ctx.client_id.utf8 = (uint8_t *)client_id_global;
    client_ctx.client_id.size = strlen(client_id_global);
    client_ctx.protocol_version = MQTT_VERSION_3_1_1;
    client_ctx.rx_buf = rx_buffer;
    client_ctx.rx_buf_size = sizeof(rx_buffer);
    client_ctx.tx_buf = tx_buffer;
    client_ctx.tx_buf_size = sizeof(tx_buffer);

    client_ctx.keepalive = p->keepalive;

    static struct mqtt_utf8 username_utf8;
    static struct mqtt_utf8 password_utf8;
    if (strlen(p->username) > 0) {
        username_utf8.utf8 = (uint8_t *)p->username;
        username_utf8.size = strlen(p->username);
        client_ctx.user_name = &username_utf8;
    }
    if (strlen(p->password) > 0) {
        password_utf8.utf8 = (uint8_t *)p->password;
        password_utf8.size = strlen(p->password);
        client_ctx.password = &password_utf8;
    }

    if (p->use_tls) {
#ifdef CONFIG_MQTT_LIB_TLS
        shell_print(sh, "[TLS] Loading credentials...");
        clear_registered_credential(TLS_CREDENTIAL_CA_CERTIFICATE);
        clear_registered_credential(TLS_CREDENTIAL_PUBLIC_CERTIFICATE);
        clear_registered_credential(TLS_CREDENTIAL_PRIVATE_KEY);

        if (strlen(p->ca_path) > 0) {
            rc = load_and_register_credential(sh, p->ca_path, TLS_CREDENTIAL_CA_CERTIFICATE);
            if (rc < 0) { shell_error(sh, "[TLS] CA load failed: %d", rc); return rc; }
            shell_print(sh, "[TLS] CA certificate loaded OK (tag 101)");
        }
        if (strlen(p->cert_path) > 0) {
            rc = load_and_register_credential(sh, p->cert_path, TLS_CREDENTIAL_PUBLIC_CERTIFICATE);
            if (rc < 0) { shell_error(sh, "[TLS] Cert load failed: %d", rc); return rc; }
        }
        if (strlen(p->key_path) > 0) {
            rc = load_and_register_credential(sh, p->key_path, TLS_CREDENTIAL_PRIVATE_KEY);
            if (rc < 0) { shell_error(sh, "[TLS] Key load failed: %d", rc); return rc; }
        }

        /* 👈 【核心修复】完全对接官方规范名称的 tls.config 参数组 */
        client_ctx.transport.type = MQTT_TRANSPORT_SECURE;
        client_ctx.transport.tls.config.sec_tag_list = sec_tag_list;
        client_ctx.transport.tls.config.sec_tag_count = ARRAY_SIZE(sec_tag_list);
        client_ctx.transport.tls.config.peer_verify = p->insecure ? TLS_PEER_VERIFY_NONE : TLS_PEER_VERIFY_REQUIRED;
        client_ctx.transport.tls.config.cipher_list = NULL;
        client_ctx.transport.tls.config.hostname = p->host;
        shell_print(sh, "[TLS] Config: peer_verify=%d, sec_tag_count=%d, hostname=%s",
                    client_ctx.transport.tls.config.peer_verify,
                    client_ctx.transport.tls.config.sec_tag_count,
                    client_ctx.transport.tls.config.hostname ? client_ctx.transport.tls.config.hostname : "(null)");
#else
        shell_error(sh, "Error: TLS options provided but TLS support is disabled (CONFIG_MQTT_LIB_TLS=n).");
        return -ENOTSUP;
#endif
    } else {
        client_ctx.transport.type = MQTT_TRANSPORT_NON_SECURE;
    }

    shell_print(sh, "Connecting to %s:%d (ID: %s, Keepalive: %d) ...", p->host, p->port, client_id_global, p->keepalive);
    shell_print(sh, "[TLS] Calling mqtt_connect (transport.type=%d)...", client_ctx.transport.type);
    rc = mqtt_connect(&client_ctx);
    if (rc != 0) {
        shell_error(sh, "Error: Underlying connection failed: %d", rc);
        return rc;
    }

    uint32_t timeout = 0;
    while (!is_connected && timeout < 5000) {
        struct zsock_pollfd fds[1] = { { .fd = get_client_fd(&client_ctx), .events = ZSOCK_POLLIN } };
        if (zsock_poll(fds, 1, 100) > 0) {
            mqtt_input(&client_ctx);
        }
        k_msleep(100);
        timeout += 100;
    }

    if (!is_connected) {
        shell_error(sh, "Error: Connection to broker timed out!");
        return -ETIMEDOUT;
    }

    shell_print(sh, "Connection successful!");
    return 0;
}

/* ==================================================================== */
/* Shell 子命令实现                                                     */
/* ==================================================================== */

/**
 * @brief mqtt_cli conn — 测试 MQTT 连接，成功后进入心跳维护循环
 */
static int cmd_mqtt_conn(const struct shell *sh, size_t argc, char **argv)
{
    sys_getopt_init();
    int c, option_index = 0;
    struct sys_getopt_state *state;
    bool help_requested = false;
    
    struct mqtt_conn_params p;
    memset(&p, 0, sizeof(p));
    strcpy(p.host, "broker.emqx.io");
    p.port = 1883;
    p.keepalive = 60;
    p.insecure = false;
    p.no_clean = false;

    while ((c = sys_getopt_long(argc, argv, "i:h:p:k:u:P:", long_options, &option_index)) != -1) {
        state = sys_getopt_state_get();
        switch (c) {
            case 'i': strncpy(p.client_id, state->optarg, sizeof(p.client_id) - 1); break;
            case 'h': strncpy(p.host, state->optarg, sizeof(p.host) - 1); break;
            case 'p': p.port = atoi(state->optarg); break;
            case 'k': p.keepalive = atoi(state->optarg); break;
            case 'u': strncpy(p.username, state->optarg, sizeof(p.username) - 1); break;
            case 'P': strncpy(p.password, state->optarg, sizeof(p.password) - 1); break;
            case OPT_KEY: strncpy(p.key_path, state->optarg, sizeof(p.key_path) - 1); p.use_tls = true; break;
            case OPT_CERT: strncpy(p.cert_path, state->optarg, sizeof(p.cert_path) - 1); p.use_tls = true; break;
            case OPT_CA: strncpy(p.ca_path, state->optarg, sizeof(p.ca_path) - 1); p.use_tls = true; break;
            case OPT_INSECURE: p.insecure = true; break;
            case OPT_KEY_PASSWORD: strncpy(p.key_password, state->optarg, sizeof(p.key_password) - 1); break;
            case OPT_NO_CLEAN: p.no_clean = true; break;
            case OPT_HELP: help_requested = true; break;
        }
    }

    if (help_requested) {
        print_conn_help(sh);
        return 0;
    }

    int rc = common_mqtt_connect(sh, &p);
    if (rc != 0) { return rc; }

    shell_print(sh, "Entered conn blocking maintenance mode. Press Ctrl+C to terminate simulation process.");
    while (is_connected) {
        struct zsock_pollfd fds[1] = { { .fd = get_client_fd(&client_ctx), .events = ZSOCK_POLLIN } };
        int res = zsock_poll(fds, 1, 500);
        if (res > 0 && (fds[0].revents & ZSOCK_POLLIN)) {
            mqtt_input(&client_ctx);
        }
        mqtt_live(&client_ctx);
    }
    return 0;
}

/**
 * @brief mqtt_cli sub — 连接 → 订阅 → 长监听（收到消息自动打印）
 */
static int cmd_mqtt_sub(const struct shell *sh, size_t argc, char **argv)
{
    sys_getopt_init();
    int c, option_index = 0;
    struct sys_getopt_state *state;
    bool help_requested = false;

    struct mqtt_conn_params p;
    memset(&p, 0, sizeof(p));
    strcpy(p.host, "broker.emqx.io");
    p.port = 1883;
    p.keepalive = 60;
    p.insecure = false;
    p.no_clean = false;

    char topic[64] = "";
    int qos = 0;

    while ((c = sys_getopt_long(argc, argv, "i:h:p:k:u:P:t:q:", long_options, &option_index)) != -1) {
        state = sys_getopt_state_get();
        switch (c) {
            case 'i': strncpy(p.client_id, state->optarg, sizeof(p.client_id) - 1); break;
            case 'h': strncpy(p.host, state->optarg, sizeof(p.host) - 1); break;
            case 'p': p.port = atoi(state->optarg); break;
            case 'k': p.keepalive = atoi(state->optarg); break;
            case 'u': strncpy(p.username, state->optarg, sizeof(p.username) - 1); break;
            case 'P': strncpy(p.password, state->optarg, sizeof(p.password) - 1); break;
            case 't': strncpy(topic, state->optarg, sizeof(topic) - 1); break;
            case 'q': qos = atoi(state->optarg); break;
            case OPT_KEY: strncpy(p.key_path, state->optarg, sizeof(p.key_path) - 1); p.use_tls = true; break;
            case OPT_CERT: strncpy(p.cert_path, state->optarg, sizeof(p.cert_path) - 1); p.use_tls = true; break;
            case OPT_CA: strncpy(p.ca_path, state->optarg, sizeof(p.ca_path) - 1); p.use_tls = true; break;
            case OPT_INSECURE: p.insecure = true; break;
            case OPT_KEY_PASSWORD: strncpy(p.key_password, state->optarg, sizeof(p.key_password) - 1); break;
            case OPT_NO_CLEAN: p.no_clean = true; break;
            case OPT_HELP: help_requested = true; break;
        }
    }

    if (help_requested) {
        print_sub_help(sh);
        return 0;
    }

    if (strlen(topic) == 0) { 
        shell_error(sh, "Error: Subscription requires topic parameter -t or --topic"); 
        return -EINVAL; 
    }

    int rc = common_mqtt_connect(sh, &p);
    if (rc != 0) { return rc; }

    static struct mqtt_topic sub_topic;
    sub_topic.topic.utf8 = (uint8_t *)topic;
    sub_topic.topic.size = strlen(topic);
    sub_topic.qos = qos;

    struct mqtt_subscription_list sub_list = {
        .list = &sub_topic,
        .list_count = 1,
        .message_id = sys_rand32_get() % 65535 + 1
    };

    shell_print(sh, "Subscribing to topic: '%s' (QoS %d) ...", topic, qos);
    rc = mqtt_subscribe(&client_ctx, &sub_list);
    if (rc != 0) { shell_error(sh, "Error: Failed to send subscription request: %d", rc); return rc; }

    shell_print(sh, "Entered sub listening state. Waiting for messages... (Press Ctrl+C to exit)");
    while (is_connected) {
        struct zsock_pollfd fds[1] = { { .fd = get_client_fd(&client_ctx), .events = ZSOCK_POLLIN } };
        int res = zsock_poll(fds, 1, 500);
        if (res > 0 && (fds[0].revents & ZSOCK_POLLIN)) {
            mqtt_input(&client_ctx);
        }
        mqtt_live(&client_ctx);
    }
    return 0;
}

/**
 * @brief mqtt_cli pub — 连接 → 发布 N 条消息（支持批量定时）→ 优雅断开
 */
static int cmd_mqtt_pub(const struct shell *sh, size_t argc, char **argv)
{
    sys_getopt_init();
    int c, option_index = 0;
    struct sys_getopt_state *state;
    bool help_requested = false;

    struct mqtt_conn_params p;
    memset(&p, 0, sizeof(p));
    strcpy(p.host, "broker.emqx.io");
    p.port = 1883;
    p.keepalive = 60;
    p.insecure = false;
    p.no_clean = false;

    char topic[64] = "";
    char message[128] = "";
    int qos = 0;
    uint32_t interval = 0;
    uint32_t limit = 1; 
    bool retain = false;
    bool dup = false;

    while ((c = sys_getopt_long(argc, argv, "i:h:p:k:u:P:t:m:q:I:L:rd", long_options, &option_index)) != -1) {
        state = sys_getopt_state_get();
        switch (c) {
            case 'i': strncpy(p.client_id, state->optarg, sizeof(p.client_id) - 1); break;
            case 'h': strncpy(p.host, state->optarg, sizeof(p.host) - 1); break;
            case 'p': p.port = atoi(state->optarg); break;
            case 'k': p.keepalive = atoi(state->optarg); break;
            case 'u': strncpy(p.username, state->optarg, sizeof(p.username) - 1); break;
            case 'P': strncpy(p.password, state->optarg, sizeof(p.password) - 1); break;
            case 't': strncpy(topic, state->optarg, sizeof(topic) - 1); break;
            case 'm': strncpy(message, state->optarg, sizeof(message) - 1); break;
            case 'q': qos = atoi(state->optarg); break;
            case 'I': interval = strtoul(state->optarg, NULL, 10); break;
            case 'L': limit = strtoul(state->optarg, NULL, 10); break;
            case 'r': retain = true; break;
            case 'd': dup = true; break;
            case OPT_KEY: strncpy(p.key_path, state->optarg, sizeof(p.key_path) - 1); p.use_tls = true; break;
            case OPT_CERT: strncpy(p.cert_path, state->optarg, sizeof(p.cert_path) - 1); p.use_tls = true; break;
            case OPT_CA: strncpy(p.ca_path, state->optarg, sizeof(p.ca_path) - 1); p.use_tls = true; break;
            case OPT_INSECURE: p.insecure = true; break;
            case OPT_KEY_PASSWORD: strncpy(p.key_password, state->optarg, sizeof(p.key_password) - 1); break;
            case OPT_NO_CLEAN: p.no_clean = true; break;
            case OPT_HELP: help_requested = true; break;
        }
    }

    if (help_requested) {
        print_pub_help(sh);
        return 0;
    }

    if (strlen(topic) == 0 || strlen(message) == 0) {
        shell_error(sh, "Error: Publish requires both topic (-t) and message (-m)");
        return -EINVAL;
    }

    int rc = common_mqtt_connect(sh, &p);
    if (rc != 0) { return rc; }

    for (uint32_t i = 0; i < limit; i++) {
        if (!is_connected) { shell_error(sh, "Publish aborted: Network disconnected!"); break; }

        struct mqtt_publish_param param = {
            .message.topic.qos = qos,
            .message.topic.topic.utf8 = (uint8_t *)topic,
            .message.topic.topic.size = strlen(topic),
            .message.payload.data = message,
            .message.payload.len = strlen(message),
            .message_id = sys_rand32_get() % 65535 + 1,
            .dup_flag = dup ? 1U : 0U,
            .retain_flag = retain ? 1U : 0U
        };

        rc = mqtt_publish(&client_ctx, &param);
        if (rc == 0) {
            shell_print(sh, "[Published %d/%d] Topic='%s' | Payload='%s'", i + 1, limit, topic, message);
        } else {
            shell_error(sh, "Error: Publish failed, error code: %d", rc);
        }

        if (interval > 0 && i < (limit - 1)) {
            uint32_t elapsed = 0;
            while (elapsed < interval) {
                struct zsock_pollfd fds[1] = { { .fd = get_client_fd(&client_ctx), .events = ZSOCK_POLLIN } };
                if (zsock_poll(fds, 1, 50) > 0) {
                    mqtt_input(&client_ctx);
                }
                mqtt_live(&client_ctx);
                k_msleep(50);
                elapsed += 50;
            }
        }
    }

    shell_print(sh, "Publish finished, gracefully disconnecting and exiting...");
    mqtt_disconnect(&client_ctx, NULL);
    k_msleep(200); 
    return 0;
}

/**
 * @brief mqtt_cli 命令注册，包含 conn / sub / pub 三个子命令
 */
SHELL_STATIC_SUBCMD_SET_CREATE(mqtt_subcmds,
    SHELL_CMD(conn, NULL, "Test connection. Params: [-i ID] [-h HOST] [-p PORT] [-k KEEP] [-u USER] [-P PASS]", cmd_mqtt_conn),
    SHELL_CMD(sub,  NULL, "Connect and subscribe. Params: -t <TOPIC> [-q QOS] [-h HOST] [-p PORT]", cmd_mqtt_sub),
    SHELL_CMD(pub,  NULL, "Connect, publish and exit. Params: -t <TOPIC> -m <MSG> [-I ms] [-L limit]", cmd_mqtt_pub),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(mqtt_cli, &mqtt_subcmds, "Standalone MQTT multi-functional console client. Use 'mqtt_cli <subcommand> --help' for details.", NULL);

/**
 * @brief 应用入口：初始化日志并进入 Zephyr Shell 交互循环
 */
int main(void)
{
    LOG_INF("System initialized successfully! Use 'mqtt_cli' for commands.");
    return 0;
}