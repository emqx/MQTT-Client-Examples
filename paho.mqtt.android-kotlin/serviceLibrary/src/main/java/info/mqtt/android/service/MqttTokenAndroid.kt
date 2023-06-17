package info.mqtt.android.service

import org.eclipse.paho.client.mqttv3.IMqttActionListener
import org.eclipse.paho.client.mqttv3.IMqttAsyncClient
import org.eclipse.paho.client.mqttv3.IMqttToken
import org.eclipse.paho.client.mqttv3.MqttException
import org.eclipse.paho.client.mqttv3.internal.wire.MqttWireMessage

internal open class MqttTokenAndroid constructor(
    private val client: MqttAndroidClient,
    private var userContext: Any?,
    private var listener: IMqttActionListener?,
    private val topics: Array<String>? = null
) : IMqttToken {
    @Volatile
    private var isComplete = false

    @Volatile
    private var lastException: MqttException? = null
    private val lock = Object()

    private var delegate: IMqttToken? = null
    private var pendingException: Throwable? = null

    @Throws(MqttException::class)
    override fun waitForCompletion() {
        synchronized(lock) {
            try {
                lock.wait()
            } catch (_: InterruptedException) {
            }
        }
        pendingException?.let { throw it }
    }

    @Throws(MqttException::class)
    override fun waitForCompletion(timeout: Long) {
        synchronized(lock) {
            try {
                lock.wait(timeout)
            } catch (_: InterruptedException) {
            }
        }
        if (!isComplete) {
            throw MqttException(MqttException.REASON_CODE_CLIENT_TIMEOUT.toInt(), Throwable("After $timeout ms"))
        }
        pendingException?.let { throw it }
    }

    fun notifyComplete() {
        synchronized(lock) {
            isComplete = true
            lock.notifyAll()
            listener?.onSuccess(this)
        }
    }

    fun notifyFailure(throwable: Throwable) {
        synchronized(lock) {
            isComplete = true
            pendingException = throwable
            lock.notifyAll()
            if (throwable is MqttException) {
                lastException = throwable
            }
            listener?.onFailure(this, throwable)
        }
    }

    override fun isComplete(): Boolean {
        return isComplete
    }

    override fun getException(): MqttException? {
        return lastException
    }

    override fun getClient(): IMqttAsyncClient {
        return client
    }

    override fun getActionCallback(): IMqttActionListener? {
        return listener
    }

    override fun setActionCallback(listener: IMqttActionListener) {
        this.listener = listener
    }

    override fun getTopics(): Array<String>? {
        return topics
    }

    override fun getUserContext(): Any? {
        return userContext
    }

    override fun setUserContext(userContext: Any) {
        this.userContext = userContext
    }

    fun setDelegate(delegate: IMqttToken?) {
        this.delegate = delegate
    }

    override fun getMessageId(): Int {
        return if (delegate != null) delegate!!.messageId else 0
    }

    override fun getResponse(): MqttWireMessage {
        return delegate!!.response
    }

    override fun getSessionPresent(): Boolean {
        return delegate!!.sessionPresent
    }

    override fun getGrantedQos(): IntArray {
        return delegate!!.grantedQos
    }

}
