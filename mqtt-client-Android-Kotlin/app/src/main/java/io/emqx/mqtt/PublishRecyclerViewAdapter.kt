package io.emqx.mqtt

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class PublishRecyclerViewAdapter(private val mValues: List<Publish>) :
    RecyclerView.Adapter<PublishRecyclerViewAdapter.ViewHolder>() {
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_publish, parent, false)
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val publish = mValues[position]
        holder.mItem = publish
        holder.mTopicView.text =
            "Topic: " + publish.topic + "  Qos: " + publish.qos + "  Retained: " + publish.isRetained
        holder.mPayloadView.text = "Payload: " + publish.payload
    }

    override fun getItemCount(): Int {
        return mValues.size
    }

    inner class ViewHolder(mView: View) : RecyclerView.ViewHolder(
        mView
    ) {
        val mTopicView: TextView = mView.findViewById(R.id.textView)
        val mPayloadView: TextView = mView.findViewById(R.id.payload)
        var mItem: Publish? = null

    }
}