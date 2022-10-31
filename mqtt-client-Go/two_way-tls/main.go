package main

import (
	"crypto/tls"
	"crypto/x509"
	"flag"
	"fmt"
	mqtt "github.com/eclipse/paho.mqtt.golang"
	"io/ioutil"
	"log"
	"time"
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
	host := flag.String("host", "127.0.0.1", "server hostname or IP")
	port := flag.Int("port", 8883, "server port")
	topic := flag.String("topic", "golang-mqtt/test", "publish/subscribe topic")
	username := flag.String("username", "emqx", "username")
	password := flag.String("password", "public", "password")
	cafile := flag.String("cafile", "",
		"path to a file containing trusted CA certificates to enable encryptedommunication.")
	cert := flag.String("cert", "",
		"client certificate for authentication, if required by server.")
	key := flag.String("key", "",
		"client certificate for authentication, if required by server.")
	flag.Parse()

	config := ConConfig{
		Broker:   fmt.Sprintf("tls://%s:%d", *host, *port),
		Topic:    *topic,
		Username: *username,
		Password: *password,
		Cafile:   *cafile,
		Cert:     *cert,
		Key:      *key,
	}
	client := mqttConnect(&config)
	go sub(client, &config)
	publish(client, &config)
}

func publish(client mqtt.Client, config *ConConfig) {
	for {
		payload := "The current time" + time.Now().Format(time.RFC3339)
		token := client.Publish(config.Topic, 0, false, payload)
		if token.Error() != nil {
			log.Printf("pub message to topic %s error:%s \n", config.Topic, token.Error())
		}
		ack := token.WaitTimeout(3 * time.Second)
		if !ack {
			log.Printf("pub message to topic %s timeout \n", config.Topic)
		}
		log.Printf("pub [%s] %s\n", config.Topic, payload)
		time.Sleep(1 * time.Second)
	}
}

func sub(client mqtt.Client, config *ConConfig) {
	token := client.Subscribe(config.Topic, 0, func(client mqtt.Client, msg mqtt.Message) {
		log.Printf("sub [%s] %s\n", msg.Topic(), string(msg.Payload()))
	})
	if token.Error() != nil {
		log.Printf("sub to message error from topic:%s \n", config.Topic)
	}
	ack := token.WaitTimeout(3 * time.Second)
	if !ack {
		log.Printf("pub message to topic timeout: %s \n", config.Topic)
	}
}

func mqttConnect(config *ConConfig) mqtt.Client {
	opts := mqtt.NewClientOptions()
	opts.AddBroker(config.Broker)
	opts.SetUsername(config.Username)
	opts.SetPassword(config.Password)
	opts.TLSConfig = loadTLSConfig(config)
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
		ca, err := ioutil.ReadFile(config.Cafile)
		if err != nil {
			log.Fatalln(err.Error())
		}
		certpool.AppendCertsFromPEM(ca)
		tlsConfig.RootCAs = certpool
	}
	if config.Cert != "" && config.Key != "" {
		clientKeyPair, err := tls.LoadX509KeyPair(config.Cert, config.Key)
		if err != nil {
			log.Fatalln(err.Error())
		}
		tlsConfig.ClientAuth = tls.RequestClientCert
		tlsConfig.Certificates = []tls.Certificate{clientKeyPair}
	}
	return &tlsConfig
}
