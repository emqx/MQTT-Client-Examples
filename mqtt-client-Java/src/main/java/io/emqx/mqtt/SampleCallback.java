package io.emqx.mqtt;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class SampleCallback implements MqttCallback {
    public void connectionLost(Throwable cause) {
        System.out.println("Connection lost: " + cause.getMessage());
    }

    public void messageArrived(String topic, MqttMessage message) {
        System.out.println("Received message: \n  Topic: " + topic + "\n  QoS: " + message.getQos() + "\n  Payload: " + new String(message.getPayload()));
    }

    public void deliveryComplete(IMqttDeliveryToken token) {
        System.out.println("deliveryComplete");
    }


}
