package info.mqtt.android.service.ping

import android.annotation.SuppressLint
import android.app.AlarmManager
import android.app.PendingIntent
import android.app.Service
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.Build
import android.os.PowerManager
import android.os.SystemClock
import info.mqtt.android.service.MqttService
import info.mqtt.android.service.MqttServiceConstants
import kotlinx.coroutines.*
import org.eclipse.paho.client.mqttv3.IMqttActionListener
import org.eclipse.paho.client.mqttv3.IMqttToken
import org.eclipse.paho.client.mqttv3.MqttException
import org.eclipse.paho.client.mqttv3.MqttPingSender
import org.eclipse.paho.client.mqttv3.internal.ClientComms
import timber.log.Timber
import kotlin.system.measureTimeMillis

/**
 * Default ping sender implementation on Android. It is based on AlarmManager.
 *
 * This class implements the [MqttPingSender] ping interface
 * allowing applications to send ping packet to server every keep alive interval.
 *
 * @see MqttPingSender
 */
internal class AlarmPingSender(val service: MqttService) : MqttPingSender {
    private var clientComms: ClientComms? = null
    private var alarmReceiver: BroadcastReceiver? = null
    private var pendingIntent: PendingIntent? = null

    private val pendingIntentFlags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
    } else {
        PendingIntent.FLAG_UPDATE_CURRENT
    }

    @Volatile
    private var hasStarted = false

    override fun init(comms: ClientComms) {
        this.clientComms = comms
        alarmReceiver = AlarmReceiver()
    }

    override fun start() {
        val action = MqttServiceConstants.PING_SENDER + clientComms!!.client.clientId
        Timber.d("Register AlarmReceiver to MqttService$action")
        service.registerReceiver(alarmReceiver, IntentFilter(action))
        pendingIntent = PendingIntent.getBroadcast(service, 0, Intent(action), pendingIntentFlags)
        schedule(clientComms!!.keepAlive)
        hasStarted = true
    }

    override fun stop() {
        Timber.d("Unregister AlarmReceiver to MqttService ${clientComms!!.client.clientId}")
        if (hasStarted) {
            if (pendingIntent != null) {
                // Cancel Alarm.
                val alarmManager = service.getSystemService(Service.ALARM_SERVICE) as AlarmManager
                alarmManager.cancel(pendingIntent)
            }
            hasStarted = false
            try {
                service.unregisterReceiver(alarmReceiver)
            } catch (e: IllegalArgumentException) {
                //Ignore unregister errors.
            }
        }
    }

    override fun schedule(delayInMilliseconds: Long) {
        val nextAlarmInMilliseconds = SystemClock.elapsedRealtime() + delayInMilliseconds
        Timber.d("Schedule next alarm at $nextAlarmInMilliseconds ms")
        val alarmManager = service.getSystemService(Service.ALARM_SERVICE) as AlarmManager
        if (Build.VERSION.SDK_INT >= 23) {
            // In SDK 23 and above, dosing will prevent setExact, setExactAndAllowWhileIdle will force
            // the device to run this task whilst dosing.
            Timber.d("Alarm schedule using setExactAndAllowWhileIdle, next: $delayInMilliseconds")
            alarmManager.setExactAndAllowWhileIdle(AlarmManager.ELAPSED_REALTIME_WAKEUP, nextAlarmInMilliseconds, pendingIntent)
        } else
            Timber.d("Alarm schedule using setExact, delay: $delayInMilliseconds")
        alarmManager.setExact(AlarmManager.ELAPSED_REALTIME_WAKEUP, nextAlarmInMilliseconds, pendingIntent)
    }

    fun backgroundExecute(comms: ClientComms?): Boolean {
        var success = false
        val token: IMqttToken? = comms?.checkForActivity(object : IMqttActionListener {
            override fun onSuccess(asyncActionToken: IMqttToken) {
                success = true
            }

            override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                Timber.d("Ping task : Failed.")
                success = false
            }
        })
        try {
            if (token != null) {
                token.waitForCompletion()
            } else {
                Timber.d("Ping background : Ping command was not sent by the client.")
            }
        } catch (e: MqttException) {
            Timber.d("Ping background : Ignore MQTT exception : ${e.message}")
        } catch (ex: Exception) {
            Timber.d("Ping background : Ignore unknown exception : ${ex.message}")
        }
        return success
    }

    /*
     * This class sends PingReq packet to MQTT broker
     */
    internal inner class AlarmReceiver : BroadcastReceiver() {
        private val wakeLockTag = MqttServiceConstants.PING_WAKELOCK + clientComms!!.client.clientId

        @SuppressLint("Wakelock")
        override fun onReceive(context: Context, intent: Intent) {
            // According to the docs, "Alarm Manager holds a CPU wake lock as
            // long as the alarm receiver's onReceive() method is executing.
            // This guarantees that the phone will not sleep until you have
            // finished handling the broadcast.", but this class still get
            // a wake lock to wait for ping finished.
            val pm = service.getSystemService(Service.POWER_SERVICE) as PowerManager
            val wakelock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, wakeLockTag)
            wakelock.acquire(TIMEOUT)

            // without blocking the main thread
            CoroutineScope(Dispatchers.IO).launch {
                measureTimeMillis {
                    @Suppress("BlockingMethodInNonBlockingContext")
                    val response = CoroutineScope(Dispatchers.IO).async {
                        return@async backgroundExecute(clientComms)
                    }.await()
                    Timber.d("Request done $response")

                    if (wakelock.isHeld) {
                        wakelock.release()
                    }
                }.also {
                    Timber.d("Completed in $it ms")
                }
            }
        }
    }

    companion object {
        private const val TIMEOUT = 10 * 60 * 1000L // 10 minutes
    }

}
