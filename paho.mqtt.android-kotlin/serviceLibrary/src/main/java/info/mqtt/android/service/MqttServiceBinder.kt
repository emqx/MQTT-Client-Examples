package info.mqtt.android.service

import android.os.Binder

class MqttServiceBinder(val service: MqttService) : Binder() {

    var activityToken: String? = null

}
