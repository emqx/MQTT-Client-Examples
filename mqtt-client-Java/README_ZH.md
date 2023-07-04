# Java MQTT 客户端使用示例

- 客户端库：[Eclipse Paho Java Client](https://github.com/eclipse/paho.mqtt.java)
- 构建工具：Maven

## 编译
```
mvn compile
```

## 运行单向 TLS 认证示例

```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttOneWayTlsSample"
```

## 运行双向 TLS 认证示例

```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttTwoWayTlsSample"
```

## 运行文件传输示例

```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttFileTransferSample"  -Dexec.args="-b tcp://broker.emqx.io:1883 -f /tmp/file.txt -u emqx -z public"
```

参数：

```
Args:
-h Help information
-b MQTT broker url [default: tcp://broker.emqx.io:1883]
-f The absolute path of the file to be uploaded [Required]
-u Username [default: emqx]
-z Password [default: public]
```

## 运行 MqttExample

```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample"

# TCP
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample" -Dexec.args="-b tcp://broker.emqx.io:1883"

# Websocket
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample" -Dexec.args="-b ws://broker.emqx.io:8083/mqtt"
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
