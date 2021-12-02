# nng-mqtt

> nng API：[libnng.3.adoc](https://github.com/nanomq/nng/blob/nng-mqtt/docs/man/libnng.3.adoc)

### 安装nng库

```shell
git clone -b nng-mqtt https://github.com/nanomq/nng.git
cd nng
mkdir build 
cd build
cmake -G Ninja ..
# 若需支持TLS: 
# cmake -G Ninja -DNNG_ENABLE_TLS=ON ..
ninja
ninja install
```

### 创建nng_socket

```c
nng_socket sock;
nng_mqtt_client_open(&sock);
```

### 创建work

#### work定义

```c
struct work {
    enum { INIT, RECV, WAIT, SEND } state;
    nng_aio *aio;
    nng_msg *msg;
    nng_ctx  ctx;
};
```

#### 为socket创建n个work

```c
static size_t nwork = 32;

struct work *works[nwork];

for (i = 0; i < nwork; i++) {
  works[i] = alloc_work(sock);
}
```

### 创建并设置mqtt connect消息体

```c
nng_msg *msg;
nng_mqtt_msg_alloc(&msg, 0);
nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_CONNECT);
nng_mqtt_msg_set_connect_keep_alive(msg, 60);
nng_mqtt_msg_set_connect_clean_session(msg, true);
```

### 创建并设置nng_dialer

#### tcp模式

```c
nng_mqtt_cb user_cb = {
  .name            = "user_cb",
  .on_connected    = connect_cb,
  .on_disconnected = disconnect_cb,
  .connect_arg     = &sock,
  .disconn_arg     = "arg",
};

// url: "mqtt-tcp://127.0.0.1:1883"
if ((rv = nng_dialer_create(&dialer, sock, url)) != 0) {
    fatal("nng_dialer_create", rv);
}
nng_dialer_set_ptr(dialer, NNG_OPT_MQTT_CONNMSG, msg);
nng_dialer_set_cb(dialer, &user_cb);
nng_dialer_start(dialer, NNG_FLAG_NONBLOCK);
```

#### tls模式

##### 设置dialer tls

```c
static int
init_dialer_tls(nng_dialer d, const char *cacert, const char *cert,
const char *key, const char *pass)
{
    nng_tls_config *cfg;
    int             rv;

    if ((rv = nng_tls_config_alloc(&cfg, NNG_TLS_MODE_CLIENT)) != 0) {
      return (rv);
    }

    if (cert != NULL && key != NULL) {
      nng_tls_config_auth_mode(cfg, NNG_TLS_AUTH_MODE_REQUIRED);
      if ((rv = nng_tls_config_own_cert(cfg, cert, key, pass)) !=
          0) {
        goto out;
      }
    } else {
      nng_tls_config_auth_mode(cfg, NNG_TLS_AUTH_MODE_NONE);
    }

    if (cacert != NULL) {
      if ((rv = nng_tls_config_ca_chain(cfg, cacert, NULL)) != 0) {
        goto out;
      }
    }

    rv = nng_dialer_setopt_ptr(d, NNG_OPT_TLS_CONFIG, cfg);

out:
    nng_tls_config_free(cfg);
    return (rv);
}	
```

##### 创建dialer

```c
nng_mqtt_cb user_cb = {
    .name            = "user_cb",
    .on_connected    = connect_cb,
    .on_disconnected = disconnect_cb,
    .connect_arg     = &sock,
    .disconn_arg     = "Args",
};

// url: "tls+mqtt-tcp://127.0.0.1:8883"
if ((rv = nng_dialer_create(&dialer, sock, url)) != 0) {
    fatal("nng_dialer_create", rv);
}
if ((rv = init_dialer_tls(dialer, ca, cert, key, pass)) != 0) {
    fatal("init_dialer_tls", rv);
}
nng_dialer_set_ptr(dialer, NNG_OPT_MQTT_CONNMSG, msg);
nng_dialer_set_cb(dialer, &user_cb);
nng_dialer_start(dialer, NNG_FLAG_NONBLOCK);
```

### 在连接回调函数中进行topic订阅

```c
#define SUB_TOPIC1 "/nanomq/msg/1"
#define SUB_TOPIC2 "/nanomq/msg/2"

static void
connect_cb(void *arg, nng_msg *msg)
{
    // Mqtt subscribe array of topic with qos
    nng_mqtt_topic_qos topic_qos[] = {
        { 
            .qos   = 0,
            .topic = {
            .buf = (uint8_t *) SUB_TOPIC1,
            .length = strlen(SUB_TOPIC1) 
            } 
        },
        { 
            .qos   = 1,
            .topic = {
            .buf = (uint8_t *) SUB_TOPIC2,
            .length = strlen(SUB_TOPIC2) 
            } 
        }
    };

    size_t topic_qos_count = sizeof(topic_qos) / sizeof(nng_mqtt_topic_qos);
    
    nng_socket sock     = *(nng_socket *) arg;
    uint8_t    ret_code = nng_mqtt_msg_get_connack_return_code(*msg*);
	printf("%s: %s(%d)\n", __FUNCTION__,
	    ret_code == 0 ? "connection established" : "connect failed", ret_code);
    
    nng_msg_free(msg);
    msg = NULL;
    
    // Connected succeed
    if (ret_code == 0) {
        nng_mqtt_msg_alloc(&msg, 0);
        nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_SUBSCRIBE);
        nng_mqtt_msg_set_subscribe_topics(
            msg, topic_qos, topic_qos_count);
        // Send subscribe message
        nng_sendmsg(sock, msg, NNG_FLAG_NONBLOCK);
    }
}
```

### 断开连接回调函数

```c
static void
disconnect_cb(void *arg, nng_msg **msg*)
{
    (void) *arg*;
    printf("%s\n",__FUNCTION__);
}
```

### 状态机回调函数

> 本实例演示从初始INIT状态进入到RECV状态后进入数据接收状态
>
> RECV状态中获取收到的PUBLISH数据
>
> WAIT状态中进行清理nng_msg并重用，组装一个PUBLISH消息，设置SEND状态并进入发送回调
>
> SEND状态获取发送结果，并返回到RECV状态中

```c
void
client_cb(void *arg)
{
	struct work *work = arg;
	nng_msg *    msg;
	int          rv;

	switch (work->state) {

	case INIT:
		work->state = RECV;
		nng_ctx_recv(work->ctx, work->aio);
		break;

	case RECV:
		if ((rv = nng_aio_result(work->aio)) != 0) {
			fatal("nng_recv_aio", rv);
			work->state = RECV;
			nng_ctx_recv(work->ctx, work->aio);
			break;
		}

		work->msg   = nng_aio_get_msg(work->aio);
		work->state = WAIT;
		nng_sleep_aio(0, work->aio);
		break;

	case WAIT:
		msg = work->msg;

		// Get PUBLISH payload and topic from msg;
		uint32_t payload_len;
		uint8_t *payload =
		    nng_mqtt_msg_get_publish_payload(msg, &payload_len);
		uint32_t    topic_len;
		const char *recv_topic =
		    nng_mqtt_msg_get_publish_topic(msg, &topic_len);

		printf("RECV: '%.*s' FROM: '%.*s'\n", payload_len,
		    (char *) payload, topic_len, recv_topic);

		uint8_t *send_data = nng_alloc(payload_len);
		memcpy(send_data, payload, payload_len);

		nng_msg_header_clear(work->msg);
		nng_msg_clear(work->msg);

		// Send payload to topic "/nanomq/msg/transfer"
		char *topic = "/nanomq/msg/transfer";
		nng_mqtt_msg_set_packet_type(work->msg, NNG_MQTT_PUBLISH);
		nng_mqtt_msg_set_publish_topic(work->msg, topic);
		nng_mqtt_msg_set_publish_payload(
		    work->msg, send_data, payload_len);

		printf("SEND: '%.*s' TO:   '%s'\n", payload_len,
		    (char *) send_data, topic);

		nng_free(send_data, payload_len);
		nng_aio_set_msg(work->aio, work->msg);
		work->msg   = NULL;
		work->state = SEND;
		nng_ctx_send(work->ctx, work->aio);
		break;

	case SEND:
		if ((rv = nng_aio_result(work->aio)) != 0) {
			nng_msg_free(work->msg);
			fatal("nng_send_aio", rv);
		}
		work->state = RECV;
		nng_ctx_recv(work->ctx, work->aio);
		break;

	default:
		fatal("bad state!", NNG_ESTATE);
		break;
	}
}
```

### 启动回调函数
```c
for (i = 0; i < nwork; i++) {
    client_cb(works[i]);
}

for (;;) {
    nng_msleep(3600000); // neither pause() nor sleep() portable
}
```

### 工程编译及运行
```shell
mkdir build
cd build
cmake -G Ninja ..
# 若需支持TLS: 
# cmake -G Ninja -DNNG_ENABLE_TLS=ON ..
ninja
```

```shell
mqtt_async: 
        -u <broker url>
        -n <number of works> (default: 32)
        -s enable ssl/tls mode (default: disable)
        -a <cafile path>
        -c <cert file path>
        -k <key file path>
        -p <key password>
```

```shell
# connect broker via tcp
./mqtt_async -u "mqtt-tcp://127.0.0.1:1883"
```

```shell
# connect broker via tls
./mqtt_async -u "tls+mqtt-tcp://127.0.0.1:8883" -s -a ca.crt -c server.crt -k server.key 
```



### 完整代码工程

[mqtt-client-nng](https://github.com/emqx/MQTT-Client-Examples/tree/master/mqtt-client-nng)


### nng源码(nng-mqtt分支)
[nng](https://github.com/nanomq/nng/tree/nng-mqtt)