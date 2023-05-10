package main

import (
	"crypto/tls"
	"crypto/x509"
	"flag"
	"fmt"
	"log"
	"os"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

type ConConfig struct {
	Broker   string
	Topic    string
	Username string
	Password string
	Cafile   string
	Cert     string
	Key      string
}

func main() {
	host := flag.String("host", "broker.emqx.io", "server hostname or IP")
	port := flag.Int("port", 8883, "server port")
	topic := flag.String("topic", "emqx/golang-mqtt", "publish/subscribe topic")
	username := flag.String("username", "emqx", "username")
	password := flag.String("password", "public", "password")
	cafile := flag.String("cafile", "",
		"path to a file containing trusted CA certificates to enable encrypted communication.")
	flag.Parse()

	config := ConConfig{
		Broker:   fmt.Sprintf("tls://%s:%d", *host, *port),
		Topic:    *topic,
		Username: *username,
		Password: *password,
		Cafile:   *cafile,
	}
	client := mqttConnect(&config)
	go publish(client, &config)
	time.Sleep(3 * time.Second)
	unSubscribe(client, &config)
	time.Sleep(3 * time.Second)
	client.Disconnect(250)
	// Deal with existing work
	log.Println("Disconnected from MQTT broker")
}

func publish(client mqtt.Client, config *ConConfig) {
	for {
		payload := "The current time " + time.Now().String()
		token := client.Publish(config.Topic, 0, false, payload)
		ack := token.WaitTimeout(3 * time.Second)
		if !ack {
			log.Printf("pub message to topic %s timeout \n", config.Topic)
		}
		if token.Error() != nil {
			log.Printf("pub message to topic %s error:%s \n", config.Topic, token.Error())
			return
		}
		log.Printf("pub [%s] %s\n", config.Topic, payload)
		time.Sleep(1 * time.Second)
	}
}

func subscribe(client mqtt.Client, config *ConConfig) {
	token := client.Subscribe(config.Topic, 0, func(client mqtt.Client, msg mqtt.Message) {
		log.Printf("sub [%s] %s\n", msg.Topic(), string(msg.Payload()))
	})
	ack := token.WaitTimeout(3 * time.Second)
	if !ack {
		log.Printf("sub message to topic timeout: %s \n", config.Topic)
	}
	if err := token.Error(); err != nil {
		log.Printf("sub to message error from topic:%s \n", config.Topic)
	}
	log.Printf("Subscribe to topic:%s successfully\n", config.Topic)
}

func unSubscribe(client mqtt.Client, config *ConConfig) {
	token := client.Unsubscribe(config.Topic)
	ack := token.WaitTimeout(3 * time.Second)
	if !ack {
		log.Printf("unsub from topic timeout: %s \n", config.Topic)
	}
	if token.Error() != nil {
		log.Printf("unsub error from topic:%s \n", config.Topic)
	}
	log.Printf("Unsubscribe from topic:%s successfully\n", config.Topic)
}

func mqttConnect(config *ConConfig) mqtt.Client {
	opts := mqtt.NewClientOptions()
	opts.AddBroker(config.Broker)
	opts.SetUsername(config.Username)
	opts.SetPassword(config.Password)
	opts.TLSConfig = loadTLSConfig(config)
	opts.OnConnect = func(client mqtt.Client) {
		log.Println("Connected to MQTT broker")
		subscribe(client, config)
	}
	opts.OnConnectionLost = func(client mqtt.Client, err error) {
		log.Println("Disconnected from MQTT broker")
	}

	client := mqtt.NewClient(opts)
	token := client.Connect()
	ack := token.WaitTimeout(5 * time.Second)
	if token.Error() != nil || !ack {
		log.Fatalf("connect%s mqtt server error: %s", config.Broker, token.Error())
	}
	return client
}

func loadTLSConfig(config *ConConfig) *tls.Config {
	// load tls config

	var tlsConfig tls.Config
	tlsConfig.InsecureSkipVerify = false
	if config.Cafile != "" {
		certpool := x509.NewCertPool()
		ca, err := os.ReadFile(config.Cafile)
		if err != nil {
			log.Fatalln(err.Error())
		}
		certpool.AppendCertsFromPEM(ca)
		tlsConfig.RootCAs = certpool
	}
	return &tlsConfig
}
