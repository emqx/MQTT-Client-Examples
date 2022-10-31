package main

import (
	"fmt"
	"github.com/eclipse/paho.mqtt.golang"
	"github.com/google/uuid"
	"sync/atomic"
	"time"
)

const brokerHost = "112.74.78.165:1883"

var mqttClient mqtt.Client

func main() {
	var ops uint64 = 0
	if err := newClient(); err != nil {
		fmt.Println("err", err)
	}
	for i := 0; i < 100; i++ {
		go func() {
			for i := 0; i < 1000; i++ {
				time.Sleep(time.Millisecond * 300)
				topic := fmt.Sprintf("$share/group/test/%s", uuid.New().String())
				token := mqttClient.Subscribe(topic, 0, func(client mqtt.Client, message mqtt.Message) {
					log.Printf("client.Subscribe Topic: %s Payload: %s \n ", topic, message.Payload())
				})
				ack := token.WaitTimeout(3 * time.Second)
				if !ack {
					fmt.Printf("sub timeout:%d topic:%s \n", 3, topic)
				}
				atomic.AddUint64(&ops, 1)
			}
		}()

		go func() {
			for i := 0; i < 10000; i++ {

				time.Sleep(time.Millisecond * 20)
				body := `{"bid":"84399fd7-93f0-4724-b257-xxxxxxx","tid":"895c12bf-b71d-4dcd-xxxxxxxx","timestamp":1665578880925,"method":"flighttask_prepare","data":{"execute_time":1665583800000,"file":{"fingerprint":"xxxxxxxxxxxxxxxxxx","url":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"},"xxxxx":"2af2e56f-xxxx-xxxx-xxxx-fb3cf0398412","task_type":1,"wayline_type":0}}`
				topic := fmt.Sprintf("/test/%s", uuid.New().String())
				if err := mqttClient.Publish(topic, 0, false, []byte(body)).Error(); err != nil {
					fmt.Printf("publish qos:0 topic:%s err:%v \n", topic, err)
				}

				token := mqttClient.Publish(topic, 2, false, []byte(body))
				if token.Error() != nil {
					fmt.Printf("publish qos:2 topic:%s err:%v \n", topic, token.Error())
				if !token.WaitTimeout(3 * time.Second) {
					fmt.Printf("pub timeout:%d topic:%s \n", 3, topic)
				}
				atomic.AddUint64(&ops, 1)
			}
		}()
	}
	for i := 0; i < 100000; i++ {
		time.Sleep(time.Second * 1)
		fmt.Println("pub and sub ------>>>", ops)
	}

}

func newClient() error {
	opts := mqtt.NewClientOptions().AddBroker(brokerHost).SetClientID(uuid.New().String())
	opts.SetKeepAlive(5 * time.Second)
	opts.SetPingTimeout(10 * time.Second)
	opts.SetWriteTimeout(3 * time.Second)
	opts.SetCleanSession(true)
	opts.SetConnectRetryInterval(5 * time.Second)
	opts.SetMaxReconnectInterval(5 * time.Second)
	opts.SetOrderMatters(false)
	opts.AutoReconnect = true
	// 重连的最大时间间隔为5s，1，2，4，5，5，5，5，5，5...
	opts.SetMaxReconnectInterval(5 * time.Second)
	opts.OnReconnecting = func(_ mqtt.Client, _ *mqtt.ClientOptions) {
		fmt.Printf("reconnect to mqtt broker %v \n", time.Now())
	}

	opts.OnConnectionLost = func(_ mqtt.Client, e error) {
		fmt.Printf("mqtt connection lost, err: %v  \n", e.Error())
	}
	opts.OnConnect = func(c mqtt.Client) {
		fmt.Printf("mqtt connection established \n")
		topicMap := make(map[string]byte)
		topicMap["/test/sub/topic"] = 0
		if token := mqttClient.SubscribeMultiple(topicMap, func(client mqtt.Client, message mqtt.Message) {
			fmt.Printf("SubscribeMultiple:\n", message.Topic())
		}); token.Wait() && token.Error() != nil {
			fmt.Printf("mqtt subscribe services reply err:\n", token.Error())
		}
	}

	mqttClient = mqtt.NewClient(opts)
	if token := mqttClient.Connect(); token.Wait() && token.Error() != nil {
		return fmt.Errorf("mqtt connection lost, err: %v", token.Error())
	}
	return nil
}
