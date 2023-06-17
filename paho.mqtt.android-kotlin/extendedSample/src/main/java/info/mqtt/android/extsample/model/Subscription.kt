package info.mqtt.android.extsample.model

import info.mqtt.android.service.QoS

class Subscription(var topic: String, var qos: QoS, var clientHandle: String, var isEnableNotifications: Boolean) {
    override fun toString(): String {
        return "Subscription{" +
                "topic='" + topic + '\'' +
                ", qos=" + qos.name + " " + qos.value +
                ", clientHandle='" + clientHandle + '\'' +
                ", enableNotifications='" + isEnableNotifications + '\'' +
                '}'
    }
}
