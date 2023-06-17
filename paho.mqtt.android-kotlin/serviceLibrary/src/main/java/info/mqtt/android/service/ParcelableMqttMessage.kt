package info.mqtt.android.service

import android.os.Parcel
import android.os.Parcelable
import android.os.Parcelable.Creator
import org.eclipse.paho.client.mqttv3.MqttMessage

/*
 * A way to flow MqttMessages via Bundles/Intents
 *
 * An application will probably use this only when receiving a message from a
 * Service in a Bundle - the necessary code will be something like this :-
 *
 * <pre>
 * `
 * private void messageArrivedAction(Bundle data) {
 * ParcelableMqttMessage message = (ParcelableMqttMessage) data
 * .getParcelable(MqttServiceConstants.CALLBACK_MESSAGE_PARCEL);
 * *Use the normal [MqttMessage] methods on the the message object.*
 * }
 *
 * It is unlikely that an application will directly use the methods which are specific to this class.
 */
class ParcelableMqttMessage : MqttMessage, Parcelable {

    var messageId: String? = null

    internal constructor(original: MqttMessage) : super(original.payload) {
        qos = original.qos
        isRetained = original.isRetained
        isDuplicate = original.isDuplicate
    }

    internal constructor(parcel: Parcel) : super(parcel.createByteArray()) {
        qos = parcel.readInt()
        val flags = parcel.createBooleanArray()
        isRetained = flags!![0]
        isDuplicate = flags[1]
        messageId = parcel.readString()
    }

    override fun describeContents() = 0

    override fun writeToParcel(parcel: Parcel, flags: Int) {
        parcel.writeByteArray(payload)
        parcel.writeInt(qos)
        parcel.writeBooleanArray(booleanArrayOf(isRetained, isDuplicate))
        parcel.writeString(messageId)
    }

    companion object CREATOR : Creator<ParcelableMqttMessage> {
        override fun createFromParcel(parcel: Parcel): ParcelableMqttMessage {
            return ParcelableMqttMessage(parcel)
        }

        override fun newArray(size: Int): Array<ParcelableMqttMessage?> {
            return arrayOfNulls(size)
        }
    }
}