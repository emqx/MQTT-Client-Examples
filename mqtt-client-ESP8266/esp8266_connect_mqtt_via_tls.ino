#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "[WIFI SSID]"; // Enter your WiFi name
const char *password = "[WIFI password]";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io"; // endpoint of emqx broker
const char *mqtt_topic = "emqx/esp8266";  // define topic
const char *mqtt_username = "your_MQTT_username";   // username for authentication
const char *mqtt_password = "your_MQTT_password";   // password for authentication
const int mqtt_port = 8883; // port of MQTT over SSL

// init wifi client
WiFiClientSecure espClient;
PubSubClient client(espClient);

/*
  The common fingerprints of EMQX broker, for reference only.
  If you are not using EMQX Cloud Serverless or public EMQX broker,
  you need to calculate the sha1 fingerprint of your server certificate
  and update the 'fingerprint' variable below.

  You can use the following command to calculate the fingerprint:
  openssl s_client -connect broker.emqx.io:8883 < /dev/null 2>/dev/null | openssl x509 -fingerprint  -sha1 -noout -in /dev/stdin
*/
// 1. fingerprint of public emqx broker. Host: broker.emqx.io
const char* fingerprint = "9C:AB:8D:07:10:4D:26:B1:C6:07:26:1C:C4:F2:6F:F1:E8:A6:0E:63";
// 2. fingerprint of EMQX Cloud Serverless. Host: *.emqxsl.com
// const char* fingerprint = "42:AE:D8:A3:42:F1:C4:1F:CD:64:9C:D7:4B:A1:EE:5B:5E:D7:E2:B5";
// 3. fingerprint of EMQX Cloud Serverless. Host: *.emqxsl.cn
// const char* fingerprint = "7E:52:D3:84:48:3C:5A:9F:A4:39:9A:8B:27:01:B1:F8:C6:AD:D4:47";

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");

  // connecting to a mqtt broker
  espClient.setFingerprint(fingerprint);
  client.setServer(mqtt_broker, mqtt_port);
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
        Serial.println(" Retrying in 5 seconds.");
        delay(5000);
    }
  }
  client.publish(mqtt_topic, "Hi EMQX I'm ESP8266 ^^");
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
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