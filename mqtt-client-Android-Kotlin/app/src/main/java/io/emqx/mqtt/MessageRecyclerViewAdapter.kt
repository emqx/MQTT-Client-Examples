package io.emqx.mqtt

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class MessageRecyclerViewAdapter(private val mValues: List<Message>) :
    RecyclerView.Adapter<MessageRecyclerViewAdapter.ViewHolder>() {
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_message, parent, false)
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        holder.mItem = mValues[position]
        holder.mTopicView.text = "Topic: " + mValues[position].topic
        holder.mContentView.text = "Payload: " + mValues[position].message.toString()
    }

    override fun getItemCount(): Int {
        return mValues.size
    }

    inner class ViewHolder(mView: View) : RecyclerView.ViewHolder(
        mView
    ) {
        val mTopicView: TextView = mView.findViewById(R.id.tv_topic)
        val mContentView: TextView = mView.findViewById(R.id.tv_payload)
        var mItem: Message? = null
        override fun toString(): String {
            return super.toString() + " '" + mContentView.text + "'"
        }

    }
}