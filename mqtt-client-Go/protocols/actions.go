package protocols

import (
	"fmt"
	"log"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

func Pub(client mqtt.Client, topic string) {
	pubClient := client
	i := 1
	for {
		payload := fmt.Sprintf("%d", i)
		pubClient.Publish(topic, 0, false, payload)
		log.Printf("pub [%s] %s\n", topic, payload)
		i += 1
		time.Sleep(1 * time.Second)
	}
}

func Sub(client mqtt.Client, topic string) {
	subClient := client
	subClient.Subscribe(topic, 0, func(subClient mqtt.Client, msg mqtt.Message) {
		log.Printf("sub [%s] %s\n", msg.Topic(), string(msg.Payload()))
	})
	for {
		time.Sleep(1 * time.Second)
	}
}

func PubSub(client mqtt.Client, topic string) {
	go Sub(client, topic)
	Pub(client, topic)
}
