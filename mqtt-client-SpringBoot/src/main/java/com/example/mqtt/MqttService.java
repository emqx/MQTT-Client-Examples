package com.example.mqtt;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

@Service
public class MqttService implements MqttCallback {

    private MqttClient client;

    @Value("${mqtt.broker-url}")
    private String brokerUrl;

    @Value("${mqtt.client-id}")
    private String clientId;

    @Value("${mqtt.username}")
    private String username;

    @Value("${mqtt.password}")
    private String password;

    public void connect() throws MqttException {
        client = new MqttClient(brokerUrl, clientId, new MemoryPersistence());
        client.setCallback(this);

        MqttConnectOptions options = new MqttConnectOptions();
        options.setUserName(username);
        options.setPassword(password.toCharArray());
        options.setAutomaticReconnect(true);
        options.setCleanSession(true);
        options.setConnectionTimeout(30);
        
        client.connect(options);
        System.out.println("Connected to MQTT broker: " + brokerUrl);
    }

    public void disconnect() throws MqttException {
        if (client != null && client.isConnected()) {
            client.disconnect();
            System.out.println("Disconnected from MQTT broker");
        }
    }

    public void subscribe(String topic) throws MqttException {
        if (client != null && client.isConnected()) {
            client.subscribe(topic);
            System.out.println("Subscribed to topic: " + topic);
        }
    }

    public void publish(String topic, String message) throws MqttException {
        if (client != null && client.isConnected()) {
            MqttMessage mqttMessage = new MqttMessage(message.getBytes());
            mqttMessage.setQos(1);
            client.publish(topic, mqttMessage);
            System.out.println("Published message: " + message + " to topic: " + topic);
        }
    }

    @Override
    public void connectionLost(Throwable cause) {
        System.out.println("Connection lost: " + cause.getMessage());
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        System.out.println("Message received on topic: " + topic + ", message: " + message.toString());
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        System.out.println("Delivery complete for token: " + token);
    }
}
