# Java MQTT 客户端使用示例

- 客户端库：[Eclipse Paho Java Client](https://github.com/eclipse/paho.mqtt.java)
- 构建工具：Maven

## 编译
```
mvn compile
``` 

## 运行
```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample""
```

参数：
```
Args:
-h Help information
-h MQTT server hostname or IP address [default: broker.emqx.io]
-p MQTT server port [default: 1883]
-a Publish/Subscribe action [default: publish]
-u Username
-p Password
-c Clean session [default: true]
-t Publish/Subscribe topic
-q QoS [default: 0]
-s SSL [default: false]
```