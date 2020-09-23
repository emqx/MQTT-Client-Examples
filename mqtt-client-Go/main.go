package main

import (
	"flag"
	"log"
	"mqtt-client-Go/protocols"
)

var Host     = flag.String("host", "broker.emqx.io", "server hostname or IP")
var Port     = flag.Int("port", 1883, "server port")
var Action   = flag.String("action", "pubsub", "pub/sub/pubsub action")
var Protocol = flag.String("protocol", "mqtt", "mqtt/mqtts/ws/wss")
var Topic    = flag.String("topic", "golang-mqtt/test", "publish/subscribe topic")
var Username = flag.String("username", "emqx", "username")
var Password = flag.String("password", "public", "password")
var Qos      = flag.Int("qos", 0, "MQTT QOS")
var Tls      = flag.Bool("tls", false, "Enable TLS/SSL")
var CaCert   = flag.String("cacert", "./broker.emqx.io-ca.crt", "tls cacert")

func main() {
	flag.Parse()
	config := protocols.Config{Host: *Host, Port: *Port, Action: *Action, Topic: *Topic, Username: *Username, Password: *Password, Qos: *Qos, Tls: *Tls, CaCert: *CaCert}
	protocol := *Protocol
	switch protocol {
	case "mqtt":
		protocols.MQTTConnection(config)
	case "mqtts":
		protocols.MQTTSConnection(config)
	case "ws":
		protocols.WSConnection(config)
	case "wss":
		protocols.WSSConnection(config)
	default:
		log.Fatalf("Unsupported protocol: %s", protocol)
	}
}
