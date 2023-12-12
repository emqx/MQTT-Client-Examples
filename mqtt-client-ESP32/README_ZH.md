# ESP32 MQTT 客户端使用示例
在本示例中我们提供了 TCP 协议连接到 MQTT Broker 示例代码。 有关 ESP32 MQTT 客户端更多使用文档，请参阅 [PubSubClient 官方文档](https://pubsubclient.knolleary.net/)。


## 前提
* Arduino ESP32 开发板管理地址: https://dl.espressif.com/dl/package_esp32_index.json


## Arduino 设置
* 安装 ESP32 开发板
```
点击 工具 -> 开发板 -> 开发板管理 -> 搜索 ESP32 -> 点击安装
```

* 安装 PubSub client
```
项目 -> 加载库 -> 管理库... -> 搜索 PubSubClient -> 安装 PubSubClient by Nick O’Leary
```

## 文件
* esp32_connect_mqtt.ino: ESP32 连接到 MQTT 服务器
* esp32_connect_mqtt_via_tls.ino: ESP32 通过 TLS 连接到 MQTT 服务器
* esp32_DS18B20_sensor_via_tls.ino: ESP32 通过 TLS 连接到 MQTT 服务器并上传 DS18B20 温度传感器数据


## TLS 配置
对于 TLS 连接示例代码，默认包含了 DigiCert Global Root G2 (broker.emqx.io-ca.crt) 和 DigiCert Global Root CA (emqxsl-ca.crt) ca_cert 证书，请依据使用场景自行修改。




