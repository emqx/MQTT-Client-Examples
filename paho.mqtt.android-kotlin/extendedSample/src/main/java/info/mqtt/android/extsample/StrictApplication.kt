package info.mqtt.android.extsample

import android.os.StrictMode
import info.hannes.logcat.LoggingApplication

class StrictApplication : LoggingApplication() {

    override fun onCreate() {
        super.onCreate()

        StrictMode.setThreadPolicy(
            StrictMode.ThreadPolicy.Builder()
                .detectDiskReads()
                .detectDiskWrites()
                .detectNetwork()
                .penaltyDialog()
                .penaltyLog()
                .build()
        )
    }
}