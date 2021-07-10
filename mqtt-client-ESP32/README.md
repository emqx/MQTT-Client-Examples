# ESP32 MQTT Client Examples
In this example we provide sample code for TCP protocol connection to the MQTT Broker.
For more documentation on using the ESP32 MQTT client, see the [Pubsubclient Documentation](https://pubsubclient.knolleary.net/).

## Prerequisites
* Arduino ESP32 development board management address: https://dl.espressif.com/dl/package_esp32_index.json


## Arduino settings

* Installing the  ESP32 Board
```
Tools -> Board -> Boards Manager... -> -> Type ESP32 in Search field -> Install
```

* Installing the PubSubClient Library
```bash
Sketch -> Include Library -> Manage Libraries... -> Type PubSubClient in Search field -> Install PubSubClient by Nick O’Leary
```


## Ino File
* esp32_connect_mqtt.ino: ESP32 connects to the MQTT broker
