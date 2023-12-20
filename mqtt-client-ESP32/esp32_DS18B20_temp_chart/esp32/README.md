# ESP32 Temperature Monitoring Project

This project involves an ESP32 device using MQTT to send temperature data. The ESP32 is configured with a DS18B20 temperature sensor and connects to an MQTT broker.

### Requirements

- ESP32 Development Board
- DS18B20 Temperature Sensor
- Arduino IDE with ESP32 support
- Wi-Fi connection

### Configuration and Setup

1. **Install the Arduino IDE**:
   Download and install the Arduino IDE from [Arduino website](https://www.arduino.cc/en/software).

2. **Add ESP32 Support in Arduino IDE**:
   Follow the instructions [here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/) to add ESP32 board support to the Arduino IDE.

3. **Install Required Libraries**:
   Open the Arduino IDE, go to `Sketch` > `Include Library` > `Manage Libraries...`. Install the following libraries:
   - `WiFi`
   - `PubSubClient`
   - `ArduinoJson`
   - `OneWire`
   - `DallasTemperature`

4. **Configure Wi-Fi and MQTT Settings**:
   Open the provided ESP32 code in Arduino IDE. Modify the following lines with your Wi-Fi and MQTT broker details:
   ```cpp
   const char* wifi_ssid = "your_wifi_ssid";
   const char* wifi_password = "your_wifi_password";
   const char* mqtt_broker = "your_mqtt_broker";
   const char* mqtt_username = "your_mqtt_username";
   const char* mqtt_password = "your_mqtt_password";
   ```

5. **Connect the DS18B20 Sensor to ESP32**:
   Connect the DS18B20 sensor to your ESP32:
   - DS18B20 VCC to ESP32 3.3V
   - DS18B20 GND to ESP32 GND
   - DS18B20 Data to ESP32 GPIO 25 (or change `one_wire_bus` in the code if using a different pin)

6. **Upload the Code**:
   Connect the ESP32 to your computer, select the correct board and port in Arduino IDE, and upload the code.

7. **Monitor the Temperature Data**:
   Once the code is uploaded, the ESP32 will start sending temperature data to the MQTT broker.
