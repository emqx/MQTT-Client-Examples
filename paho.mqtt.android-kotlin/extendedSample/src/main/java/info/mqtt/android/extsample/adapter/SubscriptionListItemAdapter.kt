package info.mqtt.android.extsample.adapter

import android.annotation.SuppressLint
import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.databinding.SubscriptionListItemBinding
import info.mqtt.android.extsample.fragments.PublishFragment
import info.mqtt.android.extsample.internal.Connection
import info.mqtt.android.extsample.model.Subscription

class SubscriptionListItemAdapter(context: Context, private val connection: Connection) :
    ArrayAdapter<Subscription>(context, R.layout.subscription_list_item, connection.getSubscriptions().toMutableList()) {

    fun refresh() {
        this.clear()
        this.addAll(connection.getSubscriptions().toMutableList())
        super.notifyDataSetChanged()
    }

    @SuppressLint("ViewHolder", "SetTextI18n")
    override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
        val binding: SubscriptionListItemBinding
        var row = convertView

        if (row == null) {
            val inflater = context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) as LayoutInflater
            binding = SubscriptionListItemBinding.inflate(inflater, parent, false)
            row = binding.root
        } else {
            binding = SubscriptionListItemBinding.bind(row)
        }

        binding.messageText.text = connection.getSubscriptions()[position].topic
        PublishFragment.DEFAULT_TOPIC = connection.getSubscriptions()[position].topic
        binding.qosLabel.text = "Qos: ${connection.getSubscriptions()[position].qos}(${connection.getSubscriptions()[position].qos.value})"
        val notifyString = context.getString(R.string.notify_text, connection.getSubscriptions()[position].isEnableNotifications.toString())
        binding.showNotificationsLabel.text = notifyString
        binding.topicDeleteImage.setOnClickListener {
            connection.unsubscribe(connection.getSubscriptions()[position])
            refresh()
        }
        return row
    }

}
