package info.mqtt.android.extsample.adapter

import android.annotation.SuppressLint
import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.databinding.MessageListItemBinding

class HistoryListItemAdapter(context: Context, private var history: List<String>) :
    ArrayAdapter<String>(context, R.layout.message_list_item, history) {

    @SuppressLint("ViewHolder", "SimpleDateFormat")
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

        binding.messageText.text = history[position]
        binding.messageTopicText.text = history[position]
        val shortDateStamp = history[position].takeLast(12)
        binding.messageDateText.text = shortDateStamp
        return row
    }
}
