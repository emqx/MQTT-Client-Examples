package info.mqtt.android.service

import org.eclipse.paho.client.mqttv3.IMqttActionListener
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken
import org.eclipse.paho.client.mqttv3.MqttMessage

/**
 * Implementation of the IMqttDeliveryToken interface for use from within the MqttAndroidClient implementation
 */
internal class MqttDeliveryTokenAndroid(
    client: MqttAndroidClient, userContext: Any?, listener: IMqttActionListener?, // The message which is being tracked by this token
    private var message: MqttMessage
) : MqttTokenAndroid(client, userContext, listener), IMqttDeliveryToken {

    override fun getMessage() = message

    fun setMessage(message: MqttMessage) {
        this.message = message
    }

    fun notifyDelivery(delivered: MqttMessage) {
        message = delivered
        super.notifyComplete()
    }
}
