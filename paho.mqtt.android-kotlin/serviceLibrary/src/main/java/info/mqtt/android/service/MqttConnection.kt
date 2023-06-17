package info.mqtt.android.service

import android.app.Service
import android.content.Context
import android.os.Bundle
import android.os.PowerManager
import android.os.PowerManager.WakeLock
import android.util.Log
import info.mqtt.android.service.ping.AlarmPingSender
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import org.eclipse.paho.client.mqttv3.*
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence
import org.eclipse.paho.client.mqttv3.persist.MqttDefaultFilePersistence
import timber.log.Timber
import java.io.File
import java.util.*

/**
 * MqttConnection holds a MqttAsyncClient {host,port,clientId} instance to perform
 * MQTT operations to MQTT broker.
 *
 * Most of the major API here is intended to implement the most general forms of
 * the methods in IMqttAsyncClient, with slight adjustments for the Android
 * environment<br></br>
 * These adjustments usually consist of adding two parameters to each method :-
 *
 *  * invocationContext - a string passed from the application to identify the
 * context of the operation (mainly included for support of the javascript API
 * implementation)
 *  * activityToken - a string passed from the Activity to relate back to a
 * callback method or other context-specific data
 *
 * Operations are very much asynchronous, so success and failure are notified by
 * packing the relevant data into Intent objects which are broadcast back to the
 * Activity via the MqttService.callbackToActivity() method.
 *
 * Constructor - create an MqttConnection to communicate with MQTT server
 *
 * @param service      our "parent" service - we make callbacks to it
 * @param serverURI    the URI of the MQTT server to which we will connect
 * @param clientId     the name by which we will identify ourselves to the MQTT
 * server
 * @param persistence  the persistence class to use to store in-flight message. If
 * null then the default persistence mechanism is used
 * @param clientHandle the "handle" by which the activity will identify us
 */
internal class MqttConnection(
    private val service: MqttService, // fields for the connection definition
    var serverURI: String, var clientId: String, private var persistence: MqttClientPersistence?, // Client handle, used for callbacks...
    var clientHandle: String
) : MqttCallbackExtended {
    // Saved sent messages and their corresponding Topics, activityTokens and
    // invocationContexts, so we can handle "deliveryComplete" callbacks from the mqttClient
    private val savedTopics: MutableMap<IMqttDeliveryToken?, String> = HashMap()
    private val savedSentMessages: MutableMap<IMqttDeliveryToken?, MqttMessage> = HashMap()
    private val savedActivityTokens: MutableMap<IMqttDeliveryToken?, String> = HashMap()
    private val savedInvocationContexts: MutableMap<IMqttDeliveryToken?, String> = HashMap()
    private val wakeLockTag = javaClass.simpleName + " " + clientId + " " + "on host " + serverURI
    private var connectOptions: MqttConnectOptions? = null

    //store connect ActivityToken for reconnect
    private var reconnectActivityToken: String? = null

    // our client object - instantiated on connect
    private var myClient: MqttAsyncClient? = null
    private var alarmPingSender: AlarmPingSender? = null

    @Volatile
    private var disconnected = true
    private var cleanSession = true

    // Indicate this connection is connecting or not.
    // This variable uses to avoid reconnect multiple times.
    @Volatile
    private var isConnecting = false
    private var wakelock: WakeLock? = null
    private var bufferOpts: DisconnectedBufferOptions? = null

    /**
     * Connect to the server specified when we were instantiated
     *
     * @param options           timeout, etc
     * @param invocationContext arbitrary data to be passed back to the application
     * @param activityToken     arbitrary identifier to be passed back to the Activity
     */
    fun connect(options: MqttConnectOptions?, invocationContext: String?, activityToken: String?) {
        connectOptions = options
        reconnectActivityToken = activityToken
        options?.let {
            cleanSession = it.isCleanSession
            if (it.isCleanSession) { // if it's a clean session,
                // discard old data
                if (service.messageDatabase.isOpen)
                    CoroutineScope(Dispatchers.IO).launch {
                        service.messageDatabase.persistenceDao().deleteClientHandle(clientHandle)
                    }
                else
                    Timber.w("Database is closed")
            }
        }
        service.traceDebug("Connecting {$serverURI} as {$clientId}")
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.CONNECT_ACTION)
        try {
            if (persistence == null) {
                // ask Android where we can put files
                // some magic Android OS has no externalFileDir or it will throw an exception, so use internal storage directly.
                var myDir: File? = null
                // use internal storage instead.
                try {
                    myDir = service.getDir(TEMP, Context.MODE_PRIVATE)
                } catch (ignored: Exception) {
                }
                if (myDir == null) {
                    try {
                        myDir = service.filesDir
                        if (myDir != null) {
                            val stringBuilder = myDir.absolutePath + File.separator
                            myDir = File(stringBuilder)
                            myDir.mkdirs()
                        }
                    } catch (e: Exception) {
                        myDir = null
                    }
                }
                persistence = if (myDir == null) {
                    // compatible very few magic Android OS.
                    MemoryPersistence()
                } else {
                    // use that to setup MQTT client persistence storage
                    MqttDefaultFilePersistence(myDir.absolutePath)
                }
            }
            val listener: IMqttActionListener = object : MqttConnectionListener(resultBundle) {
                override fun onSuccess(asyncActionToken: IMqttToken) {
                    resultBundle.putBoolean(MqttServiceConstants.SESSION_PRESENT, asyncActionToken.sessionPresent)
                    doAfterConnectSuccess(resultBundle)
                    service.traceDebug("connect success!")
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, exception?.localizedMessage)
                    resultBundle.putSerializable(MqttServiceConstants.CALLBACK_EXCEPTION, exception)
                    service.traceError("connect fail, call connect to reconnect.reason: ${exception?.message}")
                    doAfterConnectFail(resultBundle)
                }
            }
            if (myClient != null) {
                if (isConnecting) {
                    service.traceDebug("myClient != null and the client is connecting. Connect return directly.")
                    service.traceDebug("Connect return:isConnecting:$isConnecting.disconnected:$disconnected")
                } else if (!disconnected) {
                    service.traceDebug("myClient != null and the client is connected and notify!")
                    doAfterConnectSuccess(resultBundle)
                } else {
                    service.traceDebug("myClient != null and the client is not connected")
                    service.traceDebug("Do Real connect!")
                    setConnectingState(true)
                    myClient!!.connect(connectOptions, invocationContext, listener)
                }
            } else {
                alarmPingSender = AlarmPingSender(service)
                myClient = MqttAsyncClient(serverURI, clientId, persistence, alarmPingSender)
                //, null,	new AndroidHighResolutionTimer());
                myClient!!.setCallback(this)
                service.traceDebug("Do Real connect!")
                setConnectingState(true)
                myClient!!.connect(connectOptions, invocationContext, listener)
            }
        } catch (e: Exception) {
            service.traceError("Exception occurred attempting to connect: " + e.message)
            setConnectingState(false)
            handleException(resultBundle, e)
        }
    }

    private fun doAfterConnectSuccess(resultBundle: Bundle) {
        //since the device's cpu can go to sleep, acquire a wakelock and drop it later.
        acquireWakeLock()
        service.callbackToActivity(clientHandle, Status.OK, resultBundle)
        deliverBacklog()
        setConnectingState(false)
        disconnected = false
        releaseWakeLock()
    }

    override fun connectComplete(reconnect: Boolean, serverURI: String) {
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.CONNECT_EXTENDED_ACTION)
        resultBundle.putBoolean(MqttServiceConstants.CALLBACK_RECONNECT, reconnect)
        resultBundle.putString(MqttServiceConstants.CALLBACK_SERVER_URI, serverURI)
        service.callbackToActivity(clientHandle, Status.OK, resultBundle)
    }

    private fun doAfterConnectFail(resultBundle: Bundle) {
        acquireWakeLock()
        disconnected = true
        setConnectingState(false)
        service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        releaseWakeLock()
    }

    private fun handleException(resultBundle: Bundle, e: Exception) {
        resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, e.localizedMessage)
        resultBundle.putSerializable(MqttServiceConstants.CALLBACK_EXCEPTION, e)
        service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
    }

    /**
     * Attempt to deliver any outstanding messages we've received but which the
     * application hasn't acknowledged. If "cleanSession" was specified, we'll
     * have already purged any such messages from our messageStore.
     */
    private fun deliverBacklog() {
        CoroutineScope(Dispatchers.IO).launch {
            service.messageDatabase.persistenceDao().allArrived(clientHandle).forEach {
                val resultBundle = messageToBundle(it.messageId, it.topic, it.mqttMessage)
                resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.MESSAGE_ARRIVED_ACTION)
                service.callbackToActivity(clientHandle, Status.OK, resultBundle)
            }
        }
    }

    /**
     * Create a bundle containing all relevant data pertaining to a message
     *
     * @param messageId the message's identifier in the messageStore, so that a
     * callback can be made to remove it once delivered
     * @param topic     the topic on which the message was delivered
     * @param message   the message itself
     * @return the bundle
     */
    private fun messageToBundle(messageId: String?, topic: String?, message: MqttMessage): Bundle {
        val result = Bundle()
        result.putString(MqttServiceConstants.CALLBACK_MESSAGE_ID, messageId)
        result.putString(MqttServiceConstants.CALLBACK_DESTINATION_NAME, topic)
        result.putParcelable(MqttServiceConstants.CALLBACK_MESSAGE_PARCEL, ParcelableMqttMessage(message))
        return result
    }

    /**
     * Close connection from the server
     */
    fun close() {
        service.traceDebug("close()")
        try {
            myClient?.close()
        } catch (e: MqttException) {
            // Pass a new bundle, let handleException stores error messages.
            handleException(Bundle(), e)
        }
    }

    /**
     * Disconnect from the server
     *
     * @param quiesceTimeout    in milliseconds
     * @param invocationContext arbitrary data to be passed back to the application
     * @param activityToken     arbitrary string to be passed back to the activity
     */
    fun disconnect(quiesceTimeout: Long, invocationContext: String?, activityToken: String?) {
        service.traceDebug("disconnect()")
        disconnected = true
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.DISCONNECT_ACTION)
        if (myClient != null && myClient!!.isConnected) {
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                myClient!!.disconnect(quiesceTimeout, invocationContext, listener)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, NOT_CONNECTED)
            service.traceError(MqttServiceConstants.DISCONNECT_ACTION + " " + NOT_CONNECTED)
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
        if (connectOptions != null && connectOptions!!.isCleanSession) {
            // assume we'll clear the stored messages at this point
            CoroutineScope(Dispatchers.IO).launch {
                service.messageDatabase.persistenceDao().deleteClientHandle(clientHandle)
            }
        }
        releaseWakeLock()
    }

    /**
     * Disconnect from the server
     *
     * @param invocationContext arbitrary data to be passed back to the application
     * @param activityToken     arbitrary string to be passed back to the activity
     */
    fun disconnect(invocationContext: String?, activityToken: String?) {
        service.traceDebug("disconnect()")
        disconnected = true
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.DISCONNECT_ACTION)
        if (myClient != null && myClient!!.isConnected) {
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                myClient!!.disconnect(invocationContext, listener)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, NOT_CONNECTED)
            service.traceError(MqttServiceConstants.DISCONNECT_ACTION + " " + NOT_CONNECTED)
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
        if (connectOptions != null && connectOptions!!.isCleanSession) {
            // assume we'll clear the stored messages at this point
            CoroutineScope(Dispatchers.IO).launch {
                service.messageDatabase.persistenceDao().deleteClientHandle(clientHandle)
            }
        }
        releaseWakeLock()
    }

    /**
     * @return true if we are connected to an MQTT server
     */
    val isConnected: Boolean
        get() = myClient != null && myClient!!.isConnected

    /**
     * Publish a message on a topic
     *
     * @param topic             the topic on which to publish - represented as a string, not
     * an MqttTopic object
     * @param payload           the content of the message to publish
     * @param qos               the quality of service requested
     * @param retained          whether the MQTT server should retain this message
     * @param invocationContext arbitrary data to be passed back to the application
     * @param activityToken     arbitrary string to be passed back to the activity
     * @return token for tracking the operation
     */
    fun publish(
        topic: String,
        payload: ByteArray?,
        qos: QoS,
        retained: Boolean,
        invocationContext: String?,
        activityToken: String
    ): IMqttDeliveryToken? {
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.SEND_ACTION)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        var sendToken: IMqttDeliveryToken? = null
        if (myClient != null && myClient!!.isConnected) {
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                val message = MqttMessage(payload)
                message.qos = qos.value
                message.isRetained = retained
                sendToken = myClient!!.publish(topic, payload, qos.value, retained, invocationContext, listener)
                storeSendDetails(topic, message, sendToken, invocationContext, activityToken)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, NOT_CONNECTED)
            service.traceError(MqttServiceConstants.SEND_ACTION + " " + NOT_CONNECTED)
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
        return sendToken
    }

    /**
     * Publish a message on a topic
     *
     * @param topic             the topic on which to publish - represented as a string, not
     * an MqttTopic object
     * @param message           the message to publish
     * @param invocationContext arbitrary data to be passed back to the application
     * @param activityToken     arbitrary string to be passed back to the activity
     * @return token for tracking the operation
     */
    fun publish(topic: String, message: MqttMessage, invocationContext: String?, activityToken: String): IMqttDeliveryToken? {
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.SEND_ACTION)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        var sendToken: IMqttDeliveryToken? = null
        if (myClient != null && myClient!!.isConnected) {
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                sendToken = myClient!!.publish(topic, message, invocationContext, listener)
                storeSendDetails(topic, message, sendToken, invocationContext, activityToken)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else if (myClient != null && bufferOpts != null && bufferOpts!!.isBufferEnabled) {
            // Client is not connected, but buffer is enabled, so sending message
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                sendToken = myClient!!.publish(topic, message, invocationContext, listener)
                storeSendDetails(topic, message, sendToken, invocationContext, activityToken)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else {
            Timber.i("Client is not connected, so not sending message")
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, NOT_CONNECTED)
            service.traceError(MqttServiceConstants.SEND_ACTION + " " + NOT_CONNECTED)
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
        return sendToken
    }

    /**
     * Subscribe to a topic
     *
     * @param topic             a possibly wildcard topic name
     * @param qos               requested quality of service for the topic
     * @param invocationContext arbitrary data to be passed back to the application
     * @param activityToken     arbitrary identifier to be passed back to the Activity
     */
    fun subscribe(topic: String, qos: QoS, invocationContext: String?, activityToken: String) {
        service.traceDebug("subscribe({$topic},$qos,{$invocationContext}, {$activityToken}")
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.SUBSCRIBE_ACTION)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        if (myClient != null && myClient!!.isConnected) {
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                myClient!!.subscribe(topic, qos.value, invocationContext, listener)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, NOT_CONNECTED)
            service.traceError("subscribe $NOT_CONNECTED")
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
    }

    /**
     * Subscribe to one or more topics
     *
     * @param topic             a list of possibly wildcard topic names
     * @param qos               requested quality of service for each topic
     * @param invocationContext arbitrary data to be passed back to the application
     * @param activityToken     arbitrary identifier to be passed back to the Activity
     */
    fun subscribe(topic: Array<String>, qos: IntArray?, invocationContext: String?, activityToken: String) {
        service.traceDebug(
            "subscribe({" + topic.contentToString() + "}," + Arrays
                .toString(qos) + ",{" + invocationContext + "}, {" + activityToken + "}"
        )
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.SUBSCRIBE_ACTION)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        if (myClient != null && myClient!!.isConnected) {
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                myClient!!.subscribe(topic, qos, invocationContext, listener)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, NOT_CONNECTED)
            service.traceError("subscribe $NOT_CONNECTED")
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
    }

    fun subscribe(
        topicFilters: Array<String>,
        qos: Array<QoS>,
        invocationContext: String?,
        activityToken: String,
        messageListeners: Array<IMqttMessageListener>?
    ) {
        service.traceDebug(
            "subscribe({" + topicFilters.contentToString() + "}," + qos.contentToString() + ",{" +
                    invocationContext + "}, {" + activityToken + "}"
        )
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.SUBSCRIBE_ACTION)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        if (myClient != null && myClient!!.isConnected) {
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                myClient!!.subscribe(topicFilters, qos.map { it.value }.toIntArray(), null, listener, messageListeners)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, NOT_CONNECTED)
            service.traceError("subscribe $NOT_CONNECTED")
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
    }

    /**
     * Unsubscribe from a topic
     *
     * @param topic             a possibly wildcard topic name
     * @param invocationContext arbitrary data to be passed back to the application
     * @param activityToken     arbitrary identifier to be passed back to the Activity
     */
    fun unsubscribe(topic: String, invocationContext: String?, activityToken: String) {
        service.traceDebug("unsubscribe({$topic},{$invocationContext}, {$activityToken})")
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.UNSUBSCRIBE_ACTION)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        if (myClient != null && myClient!!.isConnected) {
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                myClient!!.unsubscribe(topic, invocationContext, listener)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, NOT_CONNECTED)
            service.traceError("subscribe $NOT_CONNECTED")
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
    }

    /**
     * Unsubscribe from one or more topics
     *
     * @param topic             a list of possibly wildcard topic names
     * @param invocationContext arbitrary data to be passed back to the application
     * @param activityToken     arbitrary identifier to be passed back to the Activity
     */
    fun unsubscribe(topic: Array<String>, invocationContext: String?, activityToken: String) {
        service.traceDebug("unsubscribe({" + topic.contentToString() + "},{" + invocationContext + "}, {" + activityToken + "})")
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.UNSUBSCRIBE_ACTION)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
        resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
        if (myClient != null && myClient!!.isConnected) {
            val listener: IMqttActionListener = MqttConnectionListener(resultBundle)
            try {
                myClient!!.unsubscribe(topic, invocationContext, listener)
            } catch (e: Exception) {
                handleException(resultBundle, e)
            }
        } else {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, NOT_CONNECTED)
            service.traceError("subscribe $NOT_CONNECTED")
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
    }

    /**
     * Get tokens for all outstanding deliveries for a client
     *
     * @return an array (possibly empty) of tokens
     */
    val pendingDeliveryTokens: Array<IMqttDeliveryToken>
        get() = myClient!!.pendingDeliveryTokens
    // Implement MqttCallback
    /**
     * Callback for connectionLost
     *
     * @param why the exception causing the break in communications
     */
    override fun connectionLost(why: Throwable?) {
        if (why != null) {
            service.traceDebug("connectionLost(${why.message})")
        } else {
            service.traceDebug("connectionLost(NO_REASON)")
        }
        disconnected = true
        try {
            if (!connectOptions!!.isAutomaticReconnect) {
                myClient!!.disconnect(null, object : IMqttActionListener {
                    override fun onSuccess(asyncActionToken: IMqttToken) = Unit

                    override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) = Unit
                })
            } else {
                // Using the new Automatic reconnect functionality.
                // We can't force a disconnection, but we can speed one up
                alarmPingSender!!.schedule(100)
            }
        } catch (e: Exception) {
            // ignore it - we've done our best
        }
        val resultBundle = Bundle()
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.ON_CONNECTION_LOST_ACTION)
        if (why != null) {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, why.message)
            if (why is MqttException) {
                resultBundle.putSerializable(MqttServiceConstants.CALLBACK_EXCEPTION, why)
            }
            resultBundle.putString(MqttServiceConstants.CALLBACK_EXCEPTION_STACK, Log.getStackTraceString(why))
        }
        service.callbackToActivity(clientHandle, Status.OK, resultBundle)
        // client has lost connection no need for wake lock
        releaseWakeLock()
    }

    /**
     * Callback to indicate a message has been delivered (the exact meaning of
     * "has been delivered" is dependent on the QOS value)
     *
     * @param messageToken the message token provided when the message was originally sent
     */
    override fun deliveryComplete(messageToken: IMqttDeliveryToken) {
        service.traceDebug("deliveryComplete($messageToken)")
        val resultBundle = popSendDetails(messageToken)
        if (resultBundle != null) {
            if (MqttServiceConstants.SEND_ACTION == resultBundle.getString(MqttServiceConstants.CALLBACK_ACTION)) {
                service.callbackToActivity(clientHandle, Status.OK, resultBundle)
            }
            resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.MESSAGE_DELIVERED_ACTION)
            service.callbackToActivity(clientHandle, Status.OK, resultBundle)
        }

        // this notification will have kept the connection alive but send the previously scheduled ping anyway
    }

    /**
     * Callback when a message is received
     *
     * @param topic   the topic on which the message was received
     * @param message the message itself
     */
    override fun messageArrived(topic: String, message: MqttMessage) {
        service.traceDebug("messageArrived($topic,{$message})")
        val messageId = service.messageDatabase.storeArrived(clientHandle, topic, message)
        val resultBundle = messageToBundle(messageId, topic, message)
        resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.MESSAGE_ARRIVED_ACTION)
        resultBundle.putString(MqttServiceConstants.CALLBACK_MESSAGE_ID, messageId)
        service.callbackToActivity(clientHandle, Status.OK, resultBundle)
    }

    /**
     * Removed store details of sent messages in "deliveryComplete"
     * callbacks from the mqttClient
     */
    @Synchronized
    private fun popSendDetails(messageToken: IMqttDeliveryToken): Bundle? {
        val message = savedSentMessages.remove(messageToken)
        if (message != null) { // If I don't know about the message, it's
            // irrelevant
            val topic = savedTopics.remove(messageToken)
            val activityToken = savedActivityTokens.remove(messageToken)
            val invocationContext = savedInvocationContexts.remove(messageToken)
            val resultBundle = messageToBundle(null, topic, message)
            if (activityToken != null) {
                resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.SEND_ACTION)
                resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, activityToken)
                resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, invocationContext)
            }
            return resultBundle
        }
        return null
    }

    /**
     * Store details of sent messages so we can handle "deliveryComplete"
     * callbacks from the mqttClient
     */
    @Synchronized
    private fun storeSendDetails(
        topic: String, msg: MqttMessage, messageToken: IMqttDeliveryToken?,
        invocationContext: String?, activityToken: String
    ) {
        savedTopics[messageToken] = topic
        savedSentMessages[messageToken] = msg
        savedActivityTokens[messageToken] = activityToken
        invocationContext?.let {
            savedInvocationContexts[messageToken] = it
        }

    }

    /**
     * Acquires a partial wake lock for this client
     */
    private fun acquireWakeLock() {
        if (wakelock == null) {
            val pm = service.getSystemService(Service.POWER_SERVICE) as PowerManager
            wakelock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, wakeLockTag)
        }
        wakelock!!.acquire(10 * 60 * 1000L /*10 minutes*/)
    }

    /**
     * Releases the currently held wake lock for this client
     */
    private fun releaseWakeLock() {
        if (wakelock != null && wakelock!!.isHeld) {
            wakelock!!.release()
        }
    }

    /**
     * Receive notification that we are offline<br></br>
     * if cleanSession is true, we need to regard this as a disconnection
     */
    fun offline() {
        if (!disconnected && !cleanSession) {
            val e = Exception("Android offline")
            connectionLost(e)
        }
    }

    /**
     * Reconnect<br></br>
     * Only appropriate if cleanSession is false and we were connected.
     * Declare as synchronized to avoid multiple calls to this method to send connect
     * multiple times
     */
    @Synchronized
    fun reconnect(context: Context) {
        if (myClient == null) {
            service.traceError("Reconnect myClient = null. Will not do reconnect")
            return
        }
        if (isConnecting) {
            service.traceDebug("The client is connecting. Reconnect return directly.")
            return
        }
        if (!service.isOnline(context)) {
            service.traceDebug("The network is not reachable. Will not do reconnect")
            return
        }
        if (connectOptions!!.isAutomaticReconnect) {
            //The Automatic reconnect functionality is enabled here
            Timber.i("Requesting Automatic reconnect using New Java AC")
            val resultBundle = Bundle()
            resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, reconnectActivityToken)
            resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, null)
            resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.CONNECT_ACTION)
            CoroutineScope(Dispatchers.IO).launch {
                try {
                    myClient!!.reconnect()
                } catch (ex: MqttException) {
                    CoroutineScope(Dispatchers.Main).launch {
                        Timber.e(ex, "Exception occurred attempting to reconnect: ${ex.message}")
                        setConnectingState(false)
                        handleException(resultBundle, ex)
                    }
                }
            }
        } else if (disconnected && !cleanSession) {
            // use the activityToke the same with action connect
            service.traceDebug("Do Real Reconnect!")
            val resultBundle = Bundle()
            resultBundle.putString(MqttServiceConstants.CALLBACK_ACTIVITY_TOKEN, reconnectActivityToken)
            resultBundle.putString(MqttServiceConstants.CALLBACK_INVOCATION_CONTEXT, null)
            resultBundle.putString(MqttServiceConstants.CALLBACK_ACTION, MqttServiceConstants.CONNECT_ACTION)
            try {
                val listener: IMqttActionListener = object : MqttConnectionListener(resultBundle) {
                    override fun onSuccess(asyncActionToken: IMqttToken) {
                        // since the device's cpu can go to sleep, acquire a wakelock and drop it later.
                        service.traceDebug("Reconnect Success!")
                        service.traceDebug("DeliverBacklog when reconnect.")
                        resultBundle.putBoolean(MqttServiceConstants.SESSION_PRESENT, asyncActionToken.sessionPresent)
                        doAfterConnectSuccess(resultBundle)
                    }

                    override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                        resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, exception?.localizedMessage)
                        resultBundle.putSerializable(MqttServiceConstants.CALLBACK_EXCEPTION, exception)
                        service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
                        doAfterConnectFail(resultBundle)
                    }
                }
                myClient!!.connect(connectOptions, null, listener)
                setConnectingState(true)
            } catch (e: MqttException) {
                service.traceError("Cannot reconnect to remote server." + e.message)
                setConnectingState(false)
                handleException(resultBundle, e)
            } catch (e: Exception) {
                /*  TODO: Added Due to: https://github.com/eclipse/paho.mqtt.android/issues/101
                    For some reason in a small number of cases, myClient is null here and so
				    a NullPointer Exception is thrown. This is a workaround to pass the exception
				    up to the application. myClient should not be null so more investigation is
				    required.
				*/
                service.traceError("Cannot reconnect to remote server." + e.message)
                setConnectingState(false)
                val newEx = MqttException(MqttException.REASON_CODE_UNEXPECTED_ERROR.toInt(), e.cause)
                handleException(resultBundle, newEx)
            }
        }
    }

    @Synchronized
    private fun setConnectingState(isConnecting: Boolean) {
        this.isConnecting = isConnecting
    }

    /**
     * Sets the DisconnectedBufferOptions for this client
     */
    fun setBufferOpts(bufferOpts: DisconnectedBufferOptions?) {
        this.bufferOpts = bufferOpts
        myClient!!.setBufferOpts(bufferOpts)
    }

    val bufferedMessageCount: Int
        get() = myClient!!.bufferedMessageCount

    fun getBufferedMessage(bufferIndex: Int): MqttMessage {
        return myClient!!.getBufferedMessage(bufferIndex)
    }

    fun deleteBufferedMessage(bufferIndex: Int) {
        myClient!!.deleteBufferedMessage(bufferIndex)
    }

    val inFlightMessageCount: Int
        get() = myClient!!.inFlightMessageCount

    /**
     * General-purpose IMqttActionListener for the Client context
     *
     *
     * Simply handles the basic success/failure cases for operations which don't return results
     */
    private open inner class MqttConnectionListener(private val resultBundle: Bundle) : IMqttActionListener {
        override fun onSuccess(asyncActionToken: IMqttToken) {
            service.callbackToActivity(clientHandle, Status.OK, resultBundle)
        }

        override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
            resultBundle.putString(MqttServiceConstants.CALLBACK_ERROR_MESSAGE, exception?.localizedMessage)
            resultBundle.putSerializable(MqttServiceConstants.CALLBACK_EXCEPTION, exception)
            service.callbackToActivity(clientHandle, Status.ERROR, resultBundle)
        }
    }

    companion object {
        private const val TEMP = "MqttConnection"
        private const val NOT_CONNECTED = "not connected"
    }

}
