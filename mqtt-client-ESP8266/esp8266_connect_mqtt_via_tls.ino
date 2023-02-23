#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 8883;
const char* mqtt_username = "your_MQTT_username";
const char* mqtt_password = "your_MQTT_password";
const char* mqtt_topic = "emqx/esp8266";

const char* root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"insert_your_server_cert_here" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");

  // set root ca cert
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker.");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds.");
      delay(5000);
    }
  }
  client.publish(mqtt_topic, "hello emqx");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println((char*)payload);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Reconnected to MQTT broker.");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Failed to reconnect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds.");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
