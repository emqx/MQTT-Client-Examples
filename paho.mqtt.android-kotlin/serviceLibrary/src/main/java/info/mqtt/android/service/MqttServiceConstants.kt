package info.mqtt.android.service

/**
 * Various strings used to identify operations or data in the Android MQTT
 * service, mainly used in Intents passed between Activities and the Service.
 */
internal interface MqttServiceConstants {
    companion object {
        private const val VERSION = "v0"
        const val DUPLICATE = "duplicate"
        const val RETAINED = "retained"
        const val QOS = "qos"
        const val PAYLOAD = "payload"
        const val DESTINATION_NAME = "destinationName"
        const val CLIENT_HANDLE = "clientHandle"
        const val MESSAGE_ID = "messageId"
        const val SESSION_PRESENT = "sessionPresent"

        /* Tags for actions passed between the Activity and the Service */
        const val SEND_ACTION = "send"
        const val UNSUBSCRIBE_ACTION = "unsubscribe"
        const val SUBSCRIBE_ACTION = "subscribe"
        const val DISCONNECT_ACTION = "disconnect"
        const val CONNECT_ACTION = "connect"
        const val CONNECT_EXTENDED_ACTION = "connectExtended"
        const val MESSAGE_ARRIVED_ACTION = "messageArrived"
        const val MESSAGE_DELIVERED_ACTION = "messageDelivered"
        const val ON_CONNECTION_LOST_ACTION = "onConnectionLost"
        const val TRACE_ACTION = "trace"

        /* Identifies an Intent which calls back to the Activity */
        const val CALLBACK_TO_ACTIVITY = ".callbackToActivity.$VERSION"

        /* Identifiers for extra data on Intents broadcast to the Activity */
        const val CALLBACK_ACTION = ".callbackAction"
        const val CALLBACK_STATUS = ".callbackStatus"
        const val CALLBACK_CLIENT_HANDLE = ".$CLIENT_HANDLE"
        const val CALLBACK_ERROR_MESSAGE = ".errorMessage"
        const val CALLBACK_EXCEPTION_STACK = ".exceptionStack"
        const val CALLBACK_INVOCATION_CONTEXT = "." + "invocationContext"
        const val CALLBACK_ACTIVITY_TOKEN = "." + "activityToken"
        const val CALLBACK_DESTINATION_NAME = DESTINATION_NAME
        const val CALLBACK_MESSAGE_ID = MESSAGE_ID
        const val CALLBACK_RECONNECT = ".reconnect"
        const val CALLBACK_SERVER_URI = ".serverURI"
        const val CALLBACK_MESSAGE_PARCEL = ".PARCEL"
        const val CALLBACK_TRACE_SEVERITY = ".traceSeverity"
        const val CALLBACK_ERROR_NUMBER = ".ERROR_NUMBER"
        const val CALLBACK_EXCEPTION = ".exception"

        //Intent prefix for Ping sender.
        const val PING_SENDER = ".pingSender."

        //Constant for wakelock
        const val PING_WAKELOCK = ".client."

        //Trace severity levels
        const val TRACE_ERROR = "error"
        const val TRACE_DEBUG = "debug"
        const val TRACE_EXCEPTION = "exception"
    }
}
