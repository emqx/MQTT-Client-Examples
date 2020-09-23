package protocols

import "log"

func WSSConnection(config Config) {
	client := connectByWSS(config)
	action := config.Action
	switch action {
	case "pub":
		Pub(client, config.Topic)
	case "sub":
		Sub(client, config.Topic)
	case "pubsub":
		PubSub(client, config.Topic)
	default:
		log.Fatalf("Unsupported action: %s", action)
	}
}
