package io.emqx.mqtt;

import org.eclipse.paho.client.mqttv3.MqttMessage;

public class Message {

    private String topic;

    private MqttMessage mMessage;

    public Message(String topic, MqttMessage message) {
        this.topic = topic;
        mMessage = message;
    }

    public String getTopic() {
        return topic;
    }

    public void setTopic(String topic) {
        this.topic = topic;
    }

    public MqttMessage getMessage() {
        return mMessage;
    }

    public void setMessage(MqttMessage message) {
        mMessage = message;
    }
}
