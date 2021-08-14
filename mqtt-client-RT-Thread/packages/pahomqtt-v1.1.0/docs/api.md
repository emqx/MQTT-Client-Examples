# MQTT API 介绍

## 订阅列表

Paho MQTT 中采用订阅列表的形式进行多个 Topic 的订阅，订阅列表存储在 `MQTTClient` 结构体实例中，在 MQTT 启动前配置，如下所示：

```c
... // 省略代码

MQTTClient client;

... // 省略代码

/* set subscribe table and event callback */
client.messageHandlers[0].topicFilter = MQTT_SUBTOPIC;
client.messageHandlers[0].callback = mqtt_sub_callback;
client.messageHandlers[0].qos = QOS1;
```

详细的代码讲解请参考 Samples 章节，订阅列表的最大数量可以由 `menuconfig` 中的 `Max pahomqtt subscribe topic handlers` 选项进行配置。 

## callback

paho-mqtt 使用 callback 的方式向用户提供 MQTT 的工作状态以及相关事件的处理，需要在 `MQTTClient` 结构体实例中注册使用。

|callback 名称                           |描述|
|:-----                                  |:----|
|connect_callback                        |MQTT 连接成功的回调|
|online_callback                         |MQTT 客户端成功上线的回调|
|offline_callback                        |MQTT 客户端掉线的回调|
|defaultMessageHandler                   |默认的订阅消息接收回调|
|messageHandlers[x].callback             |订阅列表中对应的订阅消息接收回调|

用户可以使用 `defaultMessageHandler` 回调默认处理接收到的订阅消息，也可以使用 `messageHandlers` 订阅列表，为 `messageHandlers` 数组中对应的每一个 Topic 提供一个独立的订阅消息接收回调。

## MQTT_URI

paho-mqtt 中提供了 uri 解析功能，可以解析域名地址、ipv4 和 ipv6 地址，可解析 `tcp://` 和 `ssl://` 类型的 URI，用户只需要按照要求填写可用的 uri 即可。

- uri 示例：

```
domain 类型
tcp://iot.eclipse.org:1883

ipv4 类型
tcp://192.168.10.1:1883
ssl://192.168.10.1:1884

ipv6 类型
tcp://[fe80::20c:29ff:fe9a:a07e]:1883
ssl://[fe80::20c:29ff:fe9a:a07e]:1884
```

## paho_mqtt_start 

```c
int paho_mqtt_start(MQTTClient *client);
```

|**参数**                           |**描述**|
|:-----                             |:----|
|client                             |MQTT 客户端实例对象|
|return                             |0 : 成功; 其他 : 失败|

该函数启动 MQTT 客户端，根据配置项订阅相应的主题。

## paho_mqtt_stop 

```c
int paho_mqtt_stop(MQTTClient *client);
```

| **参数** | **描述**              |
| :------- | :-------------------- |
| client   | MQTT 客户端实例对象   |
| return   | 0 : 成功; 其他 : 失败 |

该函数关闭 MQTT 客户端，并且释放客户端对象申请的空间。

## paho_mqtt_subscribe

```c
int paho_mqtt_subscribe(MQTTClient *client, enum QoS qos, const char *topic, subscribe_cb callback);
```

| **参数** | **描述**                         |
| :------- | :------------------------------- |
| client   | MQTT 客户端实例对象              |
| qos      | 订阅的 QOS 级别，目前只支持 QOS1 |
| topic    | 需要订阅的主题                   |
| callback | 订阅主题获取数据时执行的回调函数 |
| return   | 0 : 成功; 其他 : 失败            |

该函数用于客户端订阅新的 Topic，并且注册数据获取回调函数。

## paho_mqtt_unsubscribe

```c
int paho_mqtt_unsubscribe(MQTTClient *client, const char *topic);
```

| **参数** | **描述**              |
| :------- | :-------------------- |
| client   | MQTT 客户端实例对象   |
| topic    | 需要取消订阅的主题    |
| return   | 0 : 成功; 其他 : 失败 |

该函数用于客户端取消指定 Topic 的订阅。

## paho_mqtt_publish 

```c
int paho_mqtt_publish(MQTTClient *client, enum QoS qos, const char *topic, const char *msg_str);
```

| **参数** | **描述**                         |
| :------- | :------------------------------- |
| client   | MQTT 客户端实例对象              |
| qos      | 发送的 QOS 级别，目前只支持 QOS1 |
| topic    | 数据发送的主题                   |
| msg_str  | 需要发送的数据指针               |
| return   | 0 : 成功; 其他 : 失败            |

该函数用于客户端向指定订阅的 Topic 发送数据。

## paho_mqtt_control 

```c
int paho_mqtt_control(MQTTClient *client, int cmd, void *arg);
```

| **参数** | **描述**              |
| :------- | :-------------------- |
| client   | MQTT 客户端实例对象   |
| cmd      | 用于控制的参数类型    |
| arg      | 用于控制的参数值      |
| return   | 0 : 成功; 其他 : 失败 |

该函数用于控制客户端部分参数的值，参数的类型有如下几种：

| 参数名称                         | 描述                                           |
| -------------------------------- | ---------------------------------------------- |
| MQTT_CTRL_SET_CONN_TIMEO         | 用于设置客户端连接的超时时间                   |
| MQTT_CTRL_SET_RECONN_INTERVAL    | 用于设备客户端断线重新连接的间隔时间           |
| MQTT_CTRL_SET_KEEPALIVE_INTERVAL | 用于设置客户端发送 ping 的间隔时间             |
| MQTT_CTRL_PUBLISH_BLOCK          | 用于设置客户端发送数据时阻塞模式还是非阻塞模式 |

