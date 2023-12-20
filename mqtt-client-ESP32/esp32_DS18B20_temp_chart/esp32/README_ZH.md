# ESP32 温度监控项目

此项目涉及使用 MQTT 发送温度数据的 ESP32 设备。ESP32 配置了 DS18B20 温度传感器，并连接到 MQTT 代理。

### 系统要求

- ESP32 开发板
- DS18B20 温度传感器
- Arduino IDE 并支持 ESP32
- Wi-Fi 连接

### 配置和设置

1. **安装 Arduino IDE**：
   从 [Arduino 网站](https://www.arduino.cc/en/software) 下载并安装 Arduino IDE。

2. **在 Arduino IDE 中添加 ESP32 支持**：
   按照[这里](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)的说明，将 ESP32 板支持添加到 Arduino IDE。

3. **安装所需的库**：
   打开 Arduino IDE，转到 `Sketch` > `Include Library` > `Manage Libraries...`。安装以下库：

   - `WiFi`
   - `PubSubClient`
   - `ArduinoJson`
   - `OneWire`
   - `DallasTemperature`

4. **配置 Wi-Fi 和 MQTT 设置**：
   在 Arduino IDE 中打开提供的 ESP32 代码。用您的 Wi-Fi 和 MQTT 代理详细信息修改以下行：

   ```cpp
   const char* wifi_ssid = "your_wifi_ssid";
   const char* wifi_password = "your_wifi_password";
   const char* mqtt_broker = "your_mqtt_broker";
   const char* mqtt_username = "your_mqtt_username";
   const char* mqtt_password = "your_mqtt_password";
   ```

5. **将 DS18B20 传感器连接到 ESP32**：
   将 DS18B20 传感器连接到您的 ESP32：

   - DS18B20 VCC 接 ESP32 3.3V
   - DS18B20 GND 接 ESP32 GND
   - DS18B20 数据接 ESP32 GPIO 25（如果使用不 同的引脚，请更改代码中的 `one_wire_bus`）

6. **上传代码**：
   将 ESP32 连接到您的计算机，在 Arduino IDE 中选择正确的板和端口，然后上传代码。

7. **监控温度数据**：
   代码上传后，ESP32 将开始向 MQTT 代理发送温度数据。