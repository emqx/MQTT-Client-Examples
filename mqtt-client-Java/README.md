# Java MQTT client examples

- MQTT client：[Eclipse Paho Java Client](https://github.com/eclipse/paho.mqtt.java)
- Build tool：Maven

## Compile
```
mvn compile
```

## Run one way TLS authentication sample

```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttOneWayTlsSample"
```

## Run two way TLS authentication sample

```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttTwoWayTlsSample"
```

## Run file transfer sample

```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttFileTransferSample"  -Dexec.args="-b tcp://broker.emqx.io:1883 -f /tmp/file.txt -u emqx -z public"
```

Args:

```
Args:
-h Help information
-b MQTT broker url [default: tcp://broker.emqx.io:1883]
-f The absolute path of the file to be uploaded [Required]
-u Username [default: emqx]
-z Password [default: public]
```


## Run MqttExample

```bash
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample"

# TCP
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample" -Dexec.args="-b tcp://broker.emqx.io:1883"

# Websocket
mvn exec:java -Dexec.mainClass="io.emqx.mqtt.MqttExample" -Dexec.args="-b ws://broker.emqx.io:8083/mqtt"
```

Args:
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