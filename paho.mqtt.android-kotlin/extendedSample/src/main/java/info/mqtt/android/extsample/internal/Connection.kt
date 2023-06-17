package info.mqtt.android.extsample.internal

import android.annotation.SuppressLint
import android.content.Context
import android.content.Intent
import androidx.lifecycle.MutableLiveData
import info.mqtt.android.extsample.ActivityConstants
import info.mqtt.android.extsample.MainActivity
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.internal.Notify.notification
import info.mqtt.android.extsample.model.ReceivedMessage
import info.mqtt.android.extsample.model.Subscription
import info.mqtt.android.extsample.room.AppDatabase
import info.mqtt.android.extsample.utils.toSubscriptionEntity
import info.mqtt.android.service.MqttAndroidClient
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import org.eclipse.paho.client.mqttv3.MqttConnectOptions
import org.eclipse.paho.client.mqttv3.MqttMessage
import java.beans.PropertyChangeEvent
import java.beans.PropertyChangeListener
import java.text.SimpleDateFormat
import java.util.*

/**
 * Represents a [MqttAndroidClient] and the actions it has performed
 */
class Connection private constructor(
    private val clientHandle: String,
    var id: String,
    var hostName: String,
    var port: Int,
    private val context: Context,
    var client: MqttAndroidClient,
    private var tlsConnection: Boolean,
    var connectionOptions: MqttConnectOptions
) {
    private val listeners = ArrayList<PropertyChangeListener>()
    private val subscriptions: MutableMap<String, Subscription> = HashMap()
    val messageList = ArrayList<ReceivedMessage>()
    val historyList = ArrayList<String>()
    val messages = MutableLiveData<MutableList<ReceivedMessage>>()
    val history = MutableLiveData<MutableList<String>>()

    private var status = ConnectionStatus.NONE

    init {
        messages.postValue(arrayListOf())
        history.postValue(arrayListOf())
        addHistory("Client: $id created")
    }

    fun updateConnection(clientId: String, host: String, port: Int, tlsConnection: Boolean) {
        val uri: String = if (tlsConnection) {
            "ssl://$host:$port"
        } else {
            "tcp://$host:$port"
        }
        id = clientId
        hostName = host
        this.port = port
        this.tlsConnection = tlsConnection
        client = MqttAndroidClient(context, uri, clientId)
    }

    @SuppressLint("SimpleDateFormat")
    fun addHistory(action: String) {
        val timestamp = SimpleDateFormat("HH:mm.ss.SSS").format(Date(System.currentTimeMillis()))
        historyList.add(action + timestamp)
        history.postValue(historyList)
        notifyListeners(PropertyChangeEvent(this, ActivityConstants.historyProperty, null, null))
    }

    fun handle() = clientHandle

    val isConnected: Boolean
        get() = status == ConnectionStatus.CONNECTED

    fun changeConnectionStatus(connectionStatus: ConnectionStatus) {
        status = connectionStatus
        notifyListeners(PropertyChangeEvent(this, ActivityConstants.ConnectionStatusProperty, null, null))
    }

    override fun toString(): String {
        val sb = StringBuilder()
        sb.append(id)
        sb.append("\n ")
        when (status) {
            ConnectionStatus.CONNECTED -> sb.append(context.getString(R.string.connection_connected_to))
            ConnectionStatus.DISCONNECTED -> sb.append(context.getString(R.string.connection_disconnected_from))
            ConnectionStatus.NONE -> sb.append(context.getString(R.string.connection_unknown_status))
            ConnectionStatus.CONNECTING -> sb.append(context.getString(R.string.connection_connecting_to))
            ConnectionStatus.DISCONNECTING -> sb.append(context.getString(R.string.connection_disconnecting_from))
            ConnectionStatus.ERROR -> sb.append(context.getString(R.string.connection_error_connecting_to))
        }
        sb.append(" ")
        sb.append(hostName)
        return sb.toString()
    }

    /**
     * Compares two connection objects for equality
     * this only takes account of the client handle
     *
     * @param other The object to compare to
     * @return true if the client handles match
     */
    override fun equals(other: Any?): Boolean {
        if (other !is Connection) {
            return false
        }
        return clientHandle == other.clientHandle
    }

    /**
     * Register a [PropertyChangeListener] to this object
     *
     * @param listener the listener to register
     */
    fun registerChangeListener(listener: PropertyChangeListener) {
        listeners.add(listener)
    }

    /**
     * Notify [PropertyChangeListener] objects that the object has been updated
     *
     * @param propertyChangeEvent - The property Change event
     */
    private fun notifyListeners(propertyChangeEvent: PropertyChangeEvent) {
        listeners.forEach {
            it.propertyChange(propertyChangeEvent)
        }
    }

    /**
     * Determines if the connection is secured using SSL, returning a C style integer value
     *
     * @return 1 if SSL secured 0 if plain text
     */
    val isSSL: Int
        get() = if (tlsConnection) 1 else 0

    fun addNewSubscription(subscription: Subscription) {
        if (!subscriptions.containsKey(subscription.topic)) {
            val actionArgs = arrayOfNulls<String>(1)
            actionArgs[0] = subscription.topic
            val callback = ActionListener(context, Action.SUBSCRIBE, this, *actionArgs)
            client.subscribe(subscription.topic, subscription.qos.value, null, callback)
            CoroutineScope(Dispatchers.IO).launch {
                AppDatabase.getDatabase(context).subscriptionDao().insert(subscription.toSubscriptionEntity())
                subscriptions[subscription.topic] = subscription
            }
        }
    }

    fun unsubscribe(subscription: Subscription) {
        if (subscriptions.containsKey(subscription.topic)) {
            CoroutineScope(Dispatchers.IO).launch {
                client.unsubscribe(subscription.topic)
                subscriptions.remove(subscription.topic)
                AppDatabase.getDatabase(context).subscriptionDao().delete(subscription.toSubscriptionEntity())
            }
        }
    }

    fun getSubscriptions(): ArrayList<Subscription> {
        return ArrayList(subscriptions.values)
    }

    fun setSubscriptions(newSubs: List<Subscription>) {
        newSubs.forEach {
            subscriptions[it.topic] = it
        }
    }

    @SuppressLint("SimpleDateFormat")
    fun addMessage(topic: String, message: MqttMessage) {
        val msg = ReceivedMessage(topic, message)
        messageList.add(0, msg)
        messages.postValue(messageList)
        if (subscriptions.containsKey(topic)) {
            if (subscriptions[topic]!!.isEnableNotifications) {
                //create intent to start activity
                val intent = Intent()
                intent.setClassName(context, MainActivity::class.java.name)
                intent.putExtra("handle", clientHandle)

                SimpleDateFormat("HH:mm.ss.SSS").format(Date(System.currentTimeMillis()))
                notification(context, context.getString(R.string.notification, String(message.payload), topic), intent, R.string.notifyTitle)
            }
        }
    }

    override fun hashCode(): Int {
        var result = clientHandle.hashCode()
        result = 31 * result + id.hashCode()
        return result
    }

    enum class ConnectionStatus {
        CONNECTING, CONNECTED, DISCONNECTING, DISCONNECTED, ERROR, NONE
    }

    companion object {
        private const val FOREGROUND = true

        fun createConnection(
            clientHandle: String,
            clientId: String,
            host: String,
            port: Int,
            context: Context,
            tlsConnection: Boolean,
            connectionOptions: MqttConnectOptions
        ): Connection {
            val uri: String = if (tlsConnection) {
                "ssl://$host:$port"
            } else {
                "tcp://$host:$port"
            }
            val intent = Intent(context, MainActivity::class.java).apply {
                flags = Intent.FLAG_ACTIVITY_SINGLE_TOP
            }
            val foregroundNotification = Notify.foregroundNotification(context, clientId, intent, R.string.notifyForeground)
            val client = MqttAndroidClient(context, uri, clientId).apply {
                if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O && FOREGROUND)
                    setForegroundService(foregroundNotification)
            }
            return Connection(clientHandle, clientId, host, port, context, client, tlsConnection, connectionOptions)
        }
    }

}
