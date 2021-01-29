# ESP8266 MQTT 客户端使用示例
在本示例中我们提供了 TCP 协议连接到 MQTT Broker 示例代码。
有关 ESP8266 MQTT 客户端更多使用文档，请参阅 [ESP8266 MQTT 官方文档](https://pubsubclient.knolleary.net/)。


## 前提
* Arduino ESP8266 开发板管理地址: http://arduino.esp8266.com/stable/package_esp8266com_index.json


## 安装
```
Sketch -> Include Library -> Manage Libraries... -> Type PubSub in Search field -> Install
```

## 文件
* esp_connect_mqtt.ino: ESP8266 连接到 MQTT 服务器
* esp_mqtt_led.ino: ESP8266 远程控制 LED 灯
* temp_hum.ino: ESP8266 上报温湿度数据

## 参阅
* [ESP8266 连接到 MQTT 服务器](https://www.emqx.cn/blog/esp8266-connects-to-the-public-mqtt-broker)
* [ESP8266 远程控制 LED 灯](https://www.emqx.cn/blog/esp8266_mqtt_led)
* [ESP8266 上报温湿度数据](https://www.emqx.cn/blog/upload-sensor-data-to-mqtt-cloud-service-via-nodemcu-esp8266)



