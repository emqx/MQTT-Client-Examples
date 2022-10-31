# Golang MQTT Client Example

In this example we provide example code for TCP, Websocket, TLS, and Websockets protocols to connect to the MQTT Broker.
For more documentation on the use of the go paho-mqtt client, see the [Go Client - documentation](https://godoc.org/github.com/eclipse/paho.mqtt.golang)

## Prerequisites

* It supports go 1.13+

## Install

```shell script
go get github.com/eclipse/paho.mqtt.golang
go get github.com/gorilla/websocket
go get golang.org/x/net/proxy
```

## Run

```shell script
go run main.go -protocol mqtt -port 1883 
``` 

## Params

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