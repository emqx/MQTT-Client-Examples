package info.mqtt.android.extsample.internal

import info.mqtt.android.service.MqttTraceHandler
import timber.log.Timber

internal class MqttTraceCallback : MqttTraceHandler {
    override fun traceDebug(message: String?) {
        Timber.d(message)
    }

    override fun traceError(message: String?) {
        Timber.e(message)
    }

    override fun traceException(message: String?, e: Exception?) {
        Timber.e(e, message)
    }
}
