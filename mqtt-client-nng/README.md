# NanoSDK

### 安装NanoSDK库

```shell
git clone https://github.com/emqx/NanoSDK
cd NanoSDK
mkdir build 
cd build
cmake -G Ninja ..
# 编译TLS: 
# cmake -G Ninja -DNNG_ENABLE_TLS=ON ..
# 编译SQLITE: 
# cmake -G Ninja -DNNG_ENABLE_SQLITE=ON ..
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
nng_mqtt_msg_set_connect_proto_version(msg, opts->version);
```



### 创建并设置nng_dialer

```c

nng_mqtt_set_connect_cb(sock, connect_cb, &sock);
nng_mqtt_set_disconnect_cb(sock, disconnect_cb, NULL);

if ((rv = nng_dialer_create(&dialer, sock, opts->url)) != 0) {
    fatal("nng_dialer_create: %s", nng_strerror(rv));
}

if (opts->enable_ssl) {
    if ((rv = init_dialer_tls(dialer, opts->cacert, opts->cert,
                opts->key, opts->keypass)) != 0) {
        fatal("init_dialer_tls: %s", nng_strerror(rv));
    }
}

nng_dialer_set_ptr(dialer, NNG_OPT_MQTT_CONNMSG, msg);
nng_dialer_start(dialer, NNG_FLAG_NONBLOCK);

```


### 订阅Topic

```c
 nng_mqtt_topic_qos subscriptions[] = {
   {
       .qos   = 1,
       .topic = { 
     .buf    = (uint8_t *) SUB_TOPIC1,
           .length = strlen(SUB_TOPIC1), 
    },
   },
   {
       .qos   = 2,
       .topic = { 
     .buf    = (uint8_t *) SUB_TOPIC2,
           .length = strlen(SUB_TOPIC2), 
    },
   },
  };

 nng_mqtt_cb_opt cb_opt = {
  .sub_ack_cb = sub_callback,
 };

 nng_mqtt_client *client = nng_mqtt_client_alloc(&sock, &cb_opt, true);
 nng_mqtt_subscribe_async(client, subscriptions,
     sizeof(subscriptions) / sizeof(nng_mqtt_topic_qos), NULL);

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
# 编译TLS: 
# cmake -G Ninja -DNNG_ENABLE_TLS=ON ..
# 编译SQLITE: 
# cmake -G Ninja -DNNG_ENABLE_SQLITE=ON ..
ninja
```

```shell
mqtt_async: 
    -h, --help    
    --url           <url>    The url for mqtt broker 
                    ('mqtt-tcp://host:port' or 
                    'tls+mqtt-tcp://host:port')
                    [default: mqtt-tcp://127.0.0.1:1883]
    -n, --parallel  <number of works> (default: 32)
    -v, --version   <mqtt version> (default: 4)
    --sqlite        enable sqlite cache (default: false)
    -s, --secure    enable ssl/tls mode (default: disable)
    --cacert        <cafile path>
    -E, --cert      <cert file path>
    --key           <key file path>
    --pey_pass      <key password>
```

```shell
# connect broker via tcp
./mqtt_async --url "mqtt-tcp://127.0.0.1:1883"
```

```shell
# connect broker via tls
./mqtt_async --url "tls+mqtt-tcp://127.0.0.1:8883" -s --cacert ca.crt --cert server.crt --key server.key 
```

### NanoSDK实现案例
[nanomq_cli](https://github.com/emqx/nanomq/tree/master/nanomq_cli)

### 完整代码工程

[mqtt-client-nng](https://github.com/emqx/MQTT-Client-Examples/tree/master/mqtt-client-nng)


### NanoSDK源码
[NanoSDK](https://github.com/emqx/NanoSDK)

