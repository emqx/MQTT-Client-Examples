#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

// WiFi
const char *ssid = "mousse"; // Enter your WiFi name
const char *password = "qweqweqwe";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "temp_hum/kunming";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

// DHT11
#define DHTPIN D4
#define DHTTYPE DHT11   // DHT 11
unsigned long previousMillis = 0;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    // Set software serial baud to 115200;
    Serial.begin(115200);
    // connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    //connecting to a mqtt broker
    while (!client.connected()) {
        String client_id = "esp8266-client-";
        client_id += String(WiFi.macAddress());
        Serial.println("Connecting to public emqx mqtt broker.....");
        if (client.connect(client_id, mqtt_username, mqtt_password)) {
            Serial.println("Public emqx mqtt broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // dht11 begin
    dht.begin();
}


void loop() {
    client.loop();
    unsigned long currentMillis = millis();
    // temperature and humidity data are publish every five second
    if (currentMillis - previousMillis >= 5000) {
        previousMillis = currentMillis;
        float temp = dht.readTemperature();
        float hum = dht.readHumidity();
        // json serialize
        DynamicJsonDocument data(256);
        data["temp"] = temp;
        data["hum"] = hum;
        // publish temperature and humidity
        char json_string[256];
        serializeJson(data, json_string);
        // {"temp":23.5,"hum":55}
        Serial.println(json_string);
        client.publish(topic, json_string, false);
    }
}