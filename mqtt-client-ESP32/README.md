# ESP32 MQTT Client Examples
In this example we provide sample code for TCP protocol connection to the MQTT Broker. For more documentation on using the ESP32 MQTT client, see the [Pubsubclient Documentation](https://pubsubclient.knolleary.net/).

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
* esp32_connect_mqtt_via_tls.ino: ESP32 connects to the MQTT broker via TLS
* esp32_DS18B20_sensor_via_tls.ino: ESP32 connects to the MQTT broker via TLS and uploads DS18B20 temperature sensor data


## TLS Config
For TLS connection example code, the default includes DigiCert Global Root G2 (broker.emqx.io.crt) and DigiCert Global Root CA (emqxsl-ca.crt) ca_cert certificates, please modify according to the usage scenario.

