package info.mqtt.android

import android.util.Log
import org.eclipse.paho.client.mqttv3.IMqttAsyncClient
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken
import org.eclipse.paho.client.mqttv3.MqttCallback
import org.eclipse.paho.client.mqttv3.MqttMessage
import java.util.*
import java.util.concurrent.TimeUnit

class MqttReceiver(mqttClient: IMqttAsyncClient) : MqttCallback {
    private var isReportConnectionLoss = true
    private var connected = false
    private var clientId: String
    private val receivedMessages = Collections.synchronizedList(ArrayList<ReceivedMessage>())

    init {
        connected = true
        clientId = mqttClient.clientId
    }

    @Throws(InterruptedException::class)
    fun receiveNext(waitMilliseconds: Long): ReceivedMessage? {
        val methodName = "receiveNext"
        var receivedMessage: ReceivedMessage? = null
        if (receivedMessages.isEmpty()) {
            TimeUnit.MILLISECONDS.sleep(waitMilliseconds)
        }
        Log.i(methodName, "receiveNext time is " + Date().toString())
        Log.i(methodName, "receivedMessages = $receivedMessages")
        if (receivedMessages.isNotEmpty()) {
            Log.i(methodName, "MqttV3Receiver receive message")
            receivedMessage = receivedMessages.removeAt(0)
        }
        return receivedMessage
    }

    @Throws(InterruptedException::class)
    fun validateReceipt(sendTopic: String, expectedQos: Int, sentBytes: ByteArray?): ValidateResult {
        val waitMilliseconds: Long = 10000
        val receivedMessage = receiveNext(waitMilliseconds)
            ?: return ValidateResult(false, "No message received in waitMilliseconds=$waitMilliseconds")

        return if (sendTopic != receivedMessage.topic) {
            ValidateResult(false, " Received invalid topic sent=" + sendTopic + " received topic=" + receivedMessage.topic)
        } else if (expectedQos != receivedMessage.message.qos) {
            ValidateResult(false, "expectedQos=" + expectedQos + " != Received Qos=" + receivedMessage.message.qos)
        } else if (!Arrays.equals(sentBytes, receivedMessage.message.payload)) {
            ValidateResult(
                false,
                "Sent    :${String(sentBytes!!)}\n" +
                        "Received:${String(receivedMessage.message.payload)}\n\n" +
                        "Received invalid payload !\n"
            )
        } else
            ValidateResult(true, "")
    }

    override fun connectionLost(cause: Throwable?) {
        if (isReportConnectionLoss) {
            report("ConnectionLost: clientId=$clientId cause=$cause")
        }
        synchronized(this) {
            connected = false
        }
    }

    override fun deliveryComplete(arg0: IMqttDeliveryToken) = Unit

    override fun messageArrived(topic: String, message: MqttMessage) {
        val methodName = "messageArrived"
        Log.i(methodName, "messageArrived $topic = $message clientId = $clientId")
        Log.i(methodName, "messageArrived " + Date().toString())
        receivedMessages.add(ReceivedMessage(topic, message))
        Log.i(methodName, "receivedMessages = $receivedMessages")

        // notify();
    }

    private fun report(text: String?) {
        Log.e(this.javaClass.canonicalName, text!!)
    }

    data class ReceivedMessage(var topic: String, var message: MqttMessage)
    data class ValidateResult(val ok: Boolean, var message: String)
}