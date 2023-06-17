package info.mqtt.android.service.room.entity

import androidx.room.Entity
import androidx.room.Index
import androidx.room.PrimaryKey
import info.mqtt.android.service.QoS
import org.eclipse.paho.client.mqttv3.MqttMessage

@Entity(indices = [Index(value = ["clientHandle"])])
data class MqMessageEntity(
    @PrimaryKey val messageId: String,
    var clientHandle: String,
    var topic: String,
    var mqttMessage: MqttMessage,
    val qos: QoS,
    val retained: Boolean,
    val duplicate: Boolean,
    val timestamp: Long
)
