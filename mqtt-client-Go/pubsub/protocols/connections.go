package protocols

import (
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"io/ioutil"
	"log"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

func SetAutoReconnect(config Config, opts *mqtt.ClientOptions) {
	firstReconnectDelay, maxReconnectDelay, maxReconnectCount, reconnectRate := 1, 60, 12, 2

	opts.SetOnConnectHandler(func(client mqtt.Client) {
		log.Println("Connected to MQTT Broker!")
	})

	opts.SetConnectionLostHandler(func(client mqtt.Client, err error) {
		fmt.Printf("Connection lost: %v\nTrying to reconnect...\n", err)

		reconnectDelay := firstReconnectDelay
		for i := 0; i < maxReconnectCount; i++ {
			log.Printf("Reconnecting in %ds.\n", reconnectDelay)
			time.Sleep(time.Duration(reconnectDelay) * time.Second)
			if token := client.Connect(); token.Wait() && token.Error() != nil {
				log.Printf("Failed to reconnect: %v\n", token.Error())
			} else if client.IsConnectionOpen() {
				return
			}
			if i != maxReconnectCount-1 {
				log.Println("Reconnect failed, waiting for the next reconnection.")
			}
			reconnectDelay *= reconnectRate
			if reconnectDelay > maxReconnectDelay {
				reconnectDelay = maxReconnectDelay
			}
		}
		log.Printf("Reconnect failed after %d attempts. Exiting...", maxReconnectCount)
		ExitFlag = !client.IsConnectionOpen()
	})
}

func connectByMQTT(config Config) mqtt.Client {
	opts := mqtt.NewClientOptions()
	broker := fmt.Sprintf("tcp://%s:%d", config.Host, config.Port)
	opts.AddBroker(broker)
	opts.SetUsername(config.Username)
	opts.SetPassword(config.Password)
	SetAutoReconnect(config, opts)
	client := mqtt.NewClient(opts)
	token := client.Connect()
	for !token.WaitTimeout(3 * time.Second) {
	}
	if err := token.Error(); err != nil {
		log.Fatal(err)
	}
	return client
}

func connectByMQTTS(config Config) mqtt.Client {
	var tlsConfig tls.Config
	if config.Tls && config.CaCert == "" {
		log.Fatalln("TLS field in config is required")
	}
	certpool := x509.NewCertPool()
	ca, err := ioutil.ReadFile(config.CaCert)
	if err != nil {
		log.Fatalln(err.Error())
	}
	certpool.AppendCertsFromPEM(ca)
	tlsConfig.RootCAs = certpool

	opts := mqtt.NewClientOptions()
	broker := fmt.Sprintf("ssl://%s:%d", config.Host, config.Port)
	println(broker)
	opts.AddBroker(broker)
	opts.SetUsername(config.Username)
	opts.SetPassword(config.Password)
	opts.SetTLSConfig(&tlsConfig)
	SetAutoReconnect(config, opts)
	client := mqtt.NewClient(opts)
	token := client.Connect()
	for !token.WaitTimeout(100 * time.Millisecond) {
	}
	if err := token.Error(); err != nil {
		log.Fatal(err)
	}
	return client
}

func connectByWS(config Config) mqtt.Client {
	opts := mqtt.NewClientOptions()
	broker := fmt.Sprintf("ws://%s:%d/mqtt", config.Host, config.Port)
	opts.AddBroker(broker)
	opts.SetUsername(config.Username)
	opts.SetPassword(config.Password)
	SetAutoReconnect(config, opts)
	client := mqtt.NewClient(opts)
	token := client.Connect()
	for !token.WaitTimeout(3 * time.Second) {
	}
	if err := token.Error(); err != nil {
		log.Fatal(err)
	}
	return client
}

func connectByWSS(config Config) mqtt.Client {
	var tlsConfig tls.Config
	if config.Tls && config.CaCert == "" {
		log.Fatalln("TLS field in config is required")
	}
	certpool := x509.NewCertPool()
	ca, err := ioutil.ReadFile(config.CaCert)
	if err != nil {
		log.Fatalln(err.Error())
	}
	certpool.AppendCertsFromPEM(ca)
	tlsConfig.RootCAs = certpool

	opts := mqtt.NewClientOptions()
	broker := fmt.Sprintf("wss://%s:%d/mqtt", config.Host, config.Port)
	opts.AddBroker(broker)
	opts.SetUsername(config.Username)
	opts.SetPassword(config.Password)
	opts.SetTLSConfig(&tlsConfig)
	SetAutoReconnect(config, opts)
	client := mqtt.NewClient(opts)
	token := client.Connect()
	for !token.WaitTimeout(3 * time.Second) {
	}
	if err := token.Error(); err != nil {
		log.Fatal(err)
	}
	return client
}
