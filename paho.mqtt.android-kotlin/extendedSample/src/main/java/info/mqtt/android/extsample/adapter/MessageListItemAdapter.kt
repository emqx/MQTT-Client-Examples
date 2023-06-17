package info.mqtt.android.extsample.adapter

import android.annotation.SuppressLint
import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.databinding.MessageListItemBinding
import info.mqtt.android.extsample.model.ReceivedMessage
import info.mqtt.android.service.QoS
import java.text.SimpleDateFormat

class MessageListItemAdapter(context: Context, private var messages: List<ReceivedMessage>) :
    ArrayAdapter<ReceivedMessage>(context, R.layout.message_list_item, messages) {

    @SuppressLint("ViewHolder", "SimpleDateFormat", "SetTextI18n")
    override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
        val binding: MessageListItemBinding
        var row = convertView

        if (row == null) {
            val inflater = context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) as LayoutInflater
            binding = MessageListItemBinding.inflate(inflater, parent, false)
            row = binding.root
        } else {
            binding = MessageListItemBinding.bind(row)
        }

        binding.messageText.text = String(messages[position].message.payload)
        binding.messageTopicText.text = "${context.getString(R.string.topic_fmt)} ${messages[position].topic}"
        binding.messageInfo.text = "qos=${QoS.valueOf(messages[position].message.qos)}(${messages[position].message.qos}) " +
                "isDuplicate=${messages[position].message.isDuplicate} retained=${messages[position].message.isRetained}"
        val dateTimeFormatter = SimpleDateFormat("HH:mm:ss.sss")
        val shortDateStamp = dateTimeFormatter.format(messages[position].timestamp)
        binding.messageDateText.text = shortDateStamp
        binding.messageId.text = "ID=${messages[position].message.id}"

        binding.messageInfo.visibility = View.VISIBLE
        binding.messageId.visibility = View.VISIBLE
        return row
    }
}
