package io.emqx.mqtt

import android.view.View
import android.widget.*
import androidx.recyclerview.widget.DividerItemDecoration
import androidx.recyclerview.widget.RecyclerView
import org.eclipse.paho.client.mqttv3.IMqttActionListener
import org.eclipse.paho.client.mqttv3.IMqttToken

class SubscriptionFragment : BaseFragment() {
    private var mTopic: EditText? = null
    private var mRadioGroup: RadioGroup? = null
    private var mAdapter: SubscriptionRecyclerViewAdapter? = null
    private val mSubscriptionList: ArrayList<Subscription> = ArrayList()
    override val layoutResId: Int
        get() = R.layout.fragment_subscription_list

    override fun setUpView(view: View) {
        val recyclerView = view.findViewById<RecyclerView>(R.id.subscription_list)
        mAdapter = SubscriptionRecyclerViewAdapter(mSubscriptionList)
        recyclerView.adapter = mAdapter
        recyclerView.addItemDecoration(
            DividerItemDecoration(
                fragmentActivity,
                DividerItemDecoration.VERTICAL
            )
        )
        mTopic = view.findViewById(R.id.topic)
        mRadioGroup = view.findViewById(R.id.qos)
        val subBtn = view.findViewById<Button>(R.id.subscribe)
        subBtn.setOnClickListener {
            val subscription = subscription
            (fragmentActivity as MainActivity).subscribe(
                subscription,
                object : IMqttActionListener {
                    override fun onSuccess(asyncActionToken: IMqttToken) {
                        mSubscriptionList.add(0, subscription)
                        mAdapter!!.notifyItemInserted(0)
                    }

                    override fun onFailure(asyncActionToken: IMqttToken, exception: Throwable) {
                        Toast.makeText(fragmentActivity, "Failed to subscribe", Toast.LENGTH_SHORT)
                            .show()
                    }
                })
        }
    }

    private val subscription: Subscription
        get() {
            val topic = mTopic!!.text.toString()
            var qos = 0
            when (mRadioGroup!!.checkedRadioButtonId) {
                R.id.qos0 -> qos = 0
                R.id.qos1 -> qos = 1
                R.id.qos2 -> qos = 2
            }
            return Subscription(topic, qos)
        }

    companion object {
        fun newInstance(): SubscriptionFragment {
            return SubscriptionFragment()
        }
    }
}