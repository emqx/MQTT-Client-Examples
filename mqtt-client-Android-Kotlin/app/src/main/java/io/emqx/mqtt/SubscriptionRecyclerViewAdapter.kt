package io.emqx.mqtt

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class SubscriptionRecyclerViewAdapter(private val mValues: List<Subscription>) :
    RecyclerView.Adapter<SubscriptionRecyclerViewAdapter.ViewHolder>() {
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_subscription, parent, false)
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        holder.mItem = mValues[position]
        holder.mTopicView.text = "Topic: " + mValues[position].topic
        holder.mQosView.text = "Qos: " + mValues[position].qos + ""
    }

    override fun getItemCount(): Int {
        return mValues.size
    }

    inner class ViewHolder(mView: View) : RecyclerView.ViewHolder(
        mView
    ) {
        val mTopicView: TextView
        val mQosView: TextView
        var mItem: Subscription? = null
        override fun toString(): String {
            return super.toString() + " '" + mQosView.text + "'"
        }

        init {
            mTopicView = mView.findViewById(R.id.tv_topic)
            mQosView = mView.findViewById(R.id.tv_qos)
        }
    }
}