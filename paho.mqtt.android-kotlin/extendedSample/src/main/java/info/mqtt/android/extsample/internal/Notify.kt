package info.mqtt.android.extsample.internal

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.graphics.Color
import android.os.Build
import android.widget.Toast
import androidx.core.app.NotificationCompat
import info.mqtt.android.extsample.R

internal object Notify {

    private var MessageID = 120
    private const val channelId = "chn-01"
    private const val channelFireBaseMsg = "Chn MQTT"

    private val pendingIntentFlags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
    } else {
        PendingIntent.FLAG_UPDATE_CURRENT
    }

    fun notification(context: Context, messageString: String, intent: Intent?, notificationTitle: Int) {
        //Get the notification manage which we will use to display the notification
        val ns = Context.NOTIFICATION_SERVICE
        val notificationManager = context.getSystemService(ns) as NotificationManager

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val notificationChannel = NotificationChannel(channelId, channelFireBaseMsg, NotificationManager.IMPORTANCE_LOW)
            notificationChannel.enableLights(true)
            notificationChannel.lightColor = Color.RED
            notificationChannel.enableVibration(true)
            notificationChannel.vibrationPattern = longArrayOf(100, 200, 300, 400, 500, 400, 300, 200, 400)

            notificationManager.createNotificationChannel(notificationChannel)
        }

        val `when` = System.currentTimeMillis()

        //get the notification title from the application's strings.xml file
        val contentTitle: CharSequence = context.getString(notificationTitle)

        //the message that will be displayed as the ticker
        val ticker = "$contentTitle $messageString"

        //build the pending intent that will start the appropriate activity
        val pendingIntent = PendingIntent.getActivity(context, 0, intent, pendingIntentFlags)

        //build the notification
        val notificationCompat = NotificationCompat.Builder(context, channelId)
        notificationCompat.setAutoCancel(true)
            .setContentTitle(contentTitle)
            .setContentIntent(pendingIntent)
            .setContentText(messageString)
            .setTicker(ticker)
            .setWhen(`when`)
            .setSmallIcon(R.mipmap.ic_launcher)
        val notification = notificationCompat.build()

        notificationManager.notify(MessageID, notification)
        MessageID++
    }

    fun foregroundNotification(context: Context, connectionName: String, intent: Intent?, notificationTitle: Int): Notification {
        //Get the notification manage which we will use to display the notification
        val ns = Context.NOTIFICATION_SERVICE
        val notificationManager = context.getSystemService(ns) as NotificationManager

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val notificationChannel = NotificationChannel(channelId, channelFireBaseMsg, NotificationManager.IMPORTANCE_LOW)
            notificationChannel.enableLights(true)
            notificationChannel.lightColor = Color.RED
            notificationChannel.enableVibration(true)
            notificationChannel.vibrationPattern = longArrayOf(100, 200, 300, 400, 500, 400, 300, 200, 400)

            notificationManager.createNotificationChannel(notificationChannel)
        }

        val `when` = System.currentTimeMillis()

        //get the notification title from the application's strings.xml file
        val contentTitle: CharSequence = context.getString(notificationTitle)

        //the message that will be displayed as the ticker
        val ticker = "$contentTitle $connectionName"

        //build the pending intent that will start the appropriate activity
        val pendingIntent = PendingIntent.getActivity(context, 0, intent, pendingIntentFlags)

        //build the notification
        val notificationCompat = NotificationCompat.Builder(context, channelId)
        notificationCompat.setAutoCancel(true)
            .setContentTitle(contentTitle)
            .setContentIntent(pendingIntent)
            .setContentText(connectionName)
            .setTicker(ticker)
            .setWhen(`when`)
            .setSmallIcon(R.mipmap.ic_launcher)
        return notificationCompat.build()
    }

    fun toast(context: Context?, text: CharSequence?, duration: Int) {
        val toast = Toast.makeText(context, text, duration)
        toast.show()
    }
}
