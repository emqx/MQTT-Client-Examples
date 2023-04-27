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
	for !ExitFlag {
		payload := fmt.Sprintf("%d", i)
		if pubClient.IsConnectionOpen() {
			token := pubClient.Publish(topic, 0, false, payload)
			if token.Error() != nil {
				log.Printf("pub message to topic %s error:%s \n", topic, token.Error())
			} else {
				log.Printf("pub [%s] %s\n", topic, payload)
			}
		}
		i += 1
		time.Sleep(1 * time.Second)
	}
}

func Sub(client mqtt.Client, topic string) {
	subClient := client
	for !ExitFlag {
		//if !subClient.IsConnectionOpen() {
		subClient.Subscribe(topic, 0, func(subClient mqtt.Client, msg mqtt.Message) {
			log.Printf("sub [%s] %s\n", msg.Topic(), string(msg.Payload()))
		})
		//}
		time.Sleep(1 * time.Second)
	}
}

func PubSub(client mqtt.Client, topic string) {
	go Sub(client, topic)
	Pub(client, topic)
}
