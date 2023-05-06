# 使用 C 连接到部署

本文主要介绍如何在 C 项目中使用 `Eclipse Paho C` ，实现客户端与 MQTT 服务器的连接、订阅、收发消息等功能。

[Eclipse Paho C](https://www.eclipse.org/paho/index.php?page=clients/c/index.php) 与 Eclipse Paho
Embedded C 均为 Eclipse Paho 项目下的 C 语言客户端库（MQTT C Client），均为使用 ANSI C 编写的功能齐全的 MQTT 客户端。

## 前提条件

### 安装依赖包

```bash
sudo apt-get update
sudo apt-get -y install build-essential git cmake
```

### 编译 Paho MQTT 库

#### Linux/Mac

```bash
git clone https://github.com/eclipse/paho.mqtt.c.git
cd org.eclipse.paho.mqtt.c.git
make
sudo make install
```

#### Windows

```
mkdir build.paho

cd build.paho

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

cmake -G "NMake Makefiles" -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=TRUE ..

nmake
```

## 连接

### 连接设置

本文将使用 EMQ X 提供的 [免费公共 MQTT 服务器](https://www.emqx.com/zh/mqtt/public-mqtt5-broker)，该服务基于 EMQ X
的 [MQTT 物联网云平台](https://www.emqx.com/zh/cloud) 创建。服务器接入信息如下：

- Broker: **broker-cn.emqx.io**
- TCP Port: **1883**
- WebSocket Port: **8083**

### 导入依赖包

```c
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"
```

### 定义连接地址、认证信息以及消息发布主题

```c
#define ADDRESS     "tcp://broker.emqx.io:1883"
#define USERNAME    "emqx"
#define PASSWORD    "public"
#define CLIENTID    "c-client"
#define QOS         0
#define TOPIC       "emqx/c-test"
#define TIMEOUT     10000L
```

### 定义消息发布函数

```c
void publish(MQTTClient client, char *topic, char *payload) {
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = payload;
    message.payloadlen = strlen(payload);
    message.qos = QOS;
    message.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &message, &token);
    MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Send `%s` to topic `%s` \n", payload, TOPIC);
}
```

### 定义 on_message 回调函数，用于打印订阅主题接收的消息内容

```c
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char *payload = message->payload;
    printf("Received `%s` from `%s` topic \n", payload, topicName);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
```

### 初始化 MQTT 客户端并订阅主题

```c
int rc;
MQTTClient client;

MQTTClient_create(&client, ADDRESS, CLIENTID, 0, NULL);
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
conn_opts.username = USERNAME;
conn_opts.password = PASSWORD;
MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
    printf("Failed to connect, return code %d\n", rc);
    exit(-1);
} else {
    printf("Connected to MQTT Broker!\n");
}
// subscribe topic
MQTTClient_subscribe(client, TOPIC, QOS);
```

### 循环调用 `publish` 函数，完成消息发布

```c
char payload[16];
for (int i = 0; i < 100; i += 1) {
    // publish message to broker
    snprintf(payload, 16, "message-%d", i);
    publish(client, TOPIC, payload);
    sleep(1);
}
```

### 完整代码

```c
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://broker.emqx.io:1883"
#define USERNAME    "emqx"
#define PASSWORD    "public"
#define CLIENTID    "c-client"
#define QOS         0
#define TOPIC       "emqx/c-test"
#define TIMEOUT     10000L

void publish(MQTTClient client, char *topic, char *payload) {
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = payload;
    message.payloadlen = strlen(payload);
    message.qos = QOS;
    message.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &message, &token);
    MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Send `%s` to topic `%s` \n", payload, TOPIC);
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char *payload = message->payload;
    printf("Received `%s` from `%s` topic \n", payload, topicName);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(int argc, char *argv[]) {
    int rc;
    MQTTClient client;

    MQTTClient_create(&client, ADDRESS, CLIENTID, 0, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    } else {
        printf("Connected to MQTT Broker!\n");
    }
    // subscribe topic
    MQTTClient_subscribe(client, TOPIC, QOS);
    char payload[16];
    for (int i = 0; i < 100; i += 1) {
        // publish message to broker
        snprintf(payload, 16, "message-%d", i);
        publish(client, TOPIC, payload);
        sleep(1);
    }
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
    return rc;
}
```

## 测试验证

1. 编写 CMakeLists.txt
   ```
   cmake_minimum_required(VERSION 3.17)
   find_package(eclipse-paho-mqtt-c 1.3.9 REQUIRED)
   project(mqtt_c C)
   include_directories(/usr/local/include)
   link_directories(/usr/local/lib)
   set(CMAKE_C_STANDARD 99)
   add_executable(mqtt_c main.c)
   target_link_libraries(mqtt_c paho-mqtt3c)
   ```
2. 编译和运行代码
   ![image](https://user-images.githubusercontent.com/17525759/146886358-88018935-399f-4d1f-858d-a56c7709aa8a.png)
