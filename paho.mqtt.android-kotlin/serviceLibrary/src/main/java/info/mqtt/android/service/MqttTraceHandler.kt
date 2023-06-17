package info.mqtt.android.service

interface MqttTraceHandler {

    fun traceDebug(message: String?)

    fun traceError(message: String?)

    fun traceException(message: String?, e: Exception?)
}
