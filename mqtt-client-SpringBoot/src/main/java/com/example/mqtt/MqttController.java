package com.example.mqtt;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;
import org.eclipse.paho.client.mqttv3.MqttException;

@RestController
@RequestMapping("/mqtt")
public class MqttController {

    @Autowired
    private MqttService mqttService;

    @PostMapping("/connect")
    public String connect() {
        try {
            mqttService.connect();
            return "Connected to MQTT broker";
        } catch (MqttException e) {
            return "Failed to connect: " + e.getMessage();
        }
    }

    @PostMapping("/disconnect")
    public String disconnect() {
        try {
            mqttService.disconnect();
            return "Disconnected from MQTT broker";
        } catch (MqttException e) {
            return "Failed to disconnect: " + e.getMessage();
        }
    }

    @PostMapping("/subscribe")
    public String subscribe(@RequestParam String topic) {
        try {
            mqttService.subscribe(topic);
            return "Subscribed to topic: " + topic;
        } catch (MqttException e) {
            return "Failed to subscribe: " + e.getMessage();
        }
    }

    @PostMapping("/publish")
    public String publish(@RequestParam String topic, @RequestParam String message) {
        try {
            mqttService.publish(topic, message);
            return "Published message: " + message + " to topic: " + topic;
        } catch (MqttException e) {
            return "Failed to publish: " + e.getMessage();
        }
    }
}
