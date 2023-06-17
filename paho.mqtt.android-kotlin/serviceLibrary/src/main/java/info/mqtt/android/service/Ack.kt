package info.mqtt.android.service

import org.eclipse.paho.client.mqttv3.MqttCallback

enum class Ack {
    /**
     * As soon as the [MqttCallback.messageArrived] returns, the message has been acknowledged as received .
     */
    AUTO_ACK,

    /**
     * When [MqttCallback.messageArrived] returns, the message
     * will not be acknowledged as received, the application will have to make an acknowledgment call
     * to [MqttAndroidClient] using [MqttAndroidClient.acknowledgeMessage]
     */
    MANUAL_ACK
}