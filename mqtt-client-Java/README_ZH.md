# Java MQTT 客户端使用示例

- 客户端库：[Eclipse Paho Java Client](https://github.com/eclipse/paho.mqtt.java)
- 构建工具：Maven

## 编译
```
mvn compile
``` 

## 运行
```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample"

# TLS
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample" -Dexec.args="-b ssl://broker.emqx.io:8883"

# Websocket
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample" -Dexec.args="-b ws://broker.emqx.io:8083/mqtt"

# Websocket over TLS
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample" -Dexec.args="-b wss://broker.emqx.io:8084/mqtt"
```

参数：
```
Args:
-h Help information
-b MQTT broker url [default: tcp://broker.emqx.io:1883]
-a publish/subscribe action [default: publish]
-u Username [default: emqx]
-z Password [default: public]
-c Clean session [default: true]
-t Publish/Subscribe topic [default: test/topic]
-q QoS [default: 0]
```