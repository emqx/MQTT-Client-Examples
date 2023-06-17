package info.mqtt.android.service

import org.eclipse.paho.client.mqttv3.IMqttActionListener
import org.eclipse.paho.client.mqttv3.IMqttAsyncClient
import org.eclipse.paho.client.mqttv3.IMqttToken
import org.eclipse.paho.client.mqttv3.MqttException
import org.eclipse.paho.client.mqttv3.internal.wire.MqttWireMessage


internal class MqttConnectTokenAndroid(private val sessionPresent: Boolean) : IMqttToken {

    @Throws(MqttException::class)
    override fun waitForCompletion() = Unit

    @Throws(MqttException::class)
    override fun waitForCompletion(timeout: Long) = Unit

    override fun isComplete(): Boolean {
        return false
    }

    override fun getException(): MqttException? {
        return null
    }

    override fun getActionCallback(): IMqttActionListener? {
        return null
    }

    override fun setActionCallback(listener: IMqttActionListener) {}
    override fun getClient(): IMqttAsyncClient? {
        return null
    }

    override fun getTopics(): Array<String?> {
        return arrayOfNulls(0)
    }

    override fun getUserContext(): Any? {
        return null
    }

    override fun setUserContext(userContext: Any) {}
    override fun getMessageId(): Int {
        return 0
    }

    override fun getGrantedQos(): IntArray {
        return IntArray(0)
    }

    override fun getSessionPresent(): Boolean {
        return sessionPresent
    }

    override fun getResponse(): MqttWireMessage? {
        return null
    }
}