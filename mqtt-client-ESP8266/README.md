# ESP8266 MQTT Client Examples
In this example we provide sample code for TCP protocol connection to the MQTT Broker.
For more documentation on using the ESP8266 MQTT client, see the [ESP8266 pubsubclient Documentation](https://pubsubclient.knolleary.net/).

## Prerequisites
* Arduino ESP8266 development board management address: http://arduino.esp8266.com/stable/package_esp8266com_index.json


## Installation
```bash
Sketch -> Include Library -> Manage Libraries... -> Type PubSub in Search field -> Install
```

## Ino File
* esp_connect_mqtt.ino: ESP8266 connects to the MQTT broker
* esp_mqtt_led.ino: ESP8266 control led
* temp_hum.ino: getting started with the ESP8266 and DHT11 sensor 
* esp_mqtt_moisture.ino: Soil Moisture Sensor example  


## Other
* [ESP8266 connects to the free public MQTT broker](https://www.emqx.io/blog/esp8266-connects-to-the-public-mqtt-broker)
* [Upload sensor data to MQTT cloud service via ESP8266](https://www.emqx.io/blog/upload-sensor-data-to-mqtt-cloud-service-via-nodemcu-esp8266)