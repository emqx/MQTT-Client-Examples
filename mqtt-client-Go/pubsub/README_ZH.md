# Go MQTT 客户端使用示例
在本示例中我们提供了 TCP、Websocket、TLS、Websockets 协议连接到 MQTT Broker 示例代码。
有关 Go paho-mqtt 客户端更多使用文档，请参阅 [paho-mqtt 官方文档](https://godoc.org/github.com/eclipse/paho.mqtt.golang)。


## 前提
* 支持 go 1.13+

## 安装
```shell script
go get github.com/eclipse/paho.mqtt.golang
go get github.com/gorilla/websocket
go get golang.org/x/net/proxy
```

## 运行
```shell script
go run main.go -protocol mqtt -port 1883 
``` 

## 参数

```shell script
usage: go run main.go 
          -host string
                server hostname or IP (default "broker.emqx.io")
          -port int
                server port (default 1883)
          -protocol string
                mqtt/mqtts/ws/wss (default "mqtt")
          -action string
                pub/sub/pub_sub action (default "pub_sub")
          -topic string
                publish/subscribe topic (default "golang-mqtt/test")
          -qos int
                MQTT QOS
          -username string
                          username (default "emqx")
          -password string
                password (default "public")
          -tls
                Enable TLS/SSL
          -cacert string
                tls cacert (default "./broker.emqx.io-ca.crt")
```