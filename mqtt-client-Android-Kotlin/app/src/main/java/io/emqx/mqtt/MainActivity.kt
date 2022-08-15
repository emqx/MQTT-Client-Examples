package io.emqx.mqtt

import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import androidx.viewpager.widget.ViewPager
import com.google.android.material.tabs.TabLayout
import org.eclipse.paho.android.service.MqttAndroidClient
import org.eclipse.paho.client.mqttv3.*

class MainActivity : AppCompatActivity(), MqttCallback {
    private var mClient: MqttAndroidClient? = null
    private val mFragmentList: MutableList<Fragment> = ArrayList()
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        mFragmentList.add(ConnectionFragment.newInstance())
        mFragmentList.add(SubscriptionFragment.newInstance())
        mFragmentList.add(PublishFragment.newInstance())
        mFragmentList.add(MessageFragment.newInstance())
        val sectionsPagerAdapter = SectionsPagerAdapter(supportFragmentManager, this, mFragmentList)
        val viewPager = findViewById<ViewPager>(R.id.view_pager)
        viewPager.offscreenPageLimit = 3
        viewPager.adapter = sectionsPagerAdapter
        val tabs = findViewById<TabLayout>(R.id.tabs)
        tabs.setupWithViewPager(viewPager)
    }

    fun connect(connection: Connection, listener: IMqttActionListener?) {
        mClient = connection.getMqttAndroidClient(this)
        try {
            mClient!!.connect(connection.mqttConnectOptions, null, listener)
            mClient!!.setCallback(this)
        } catch (e: MqttException) {
            e.printStackTrace()
            Toast.makeText(this, "Failed to connect", Toast.LENGTH_SHORT).show()
        }
    }

    fun disconnect() {
        if (notConnected(true)) {
            return
        }
        try {
            mClient!!.disconnect()
        } catch (e: MqttException) {
            e.printStackTrace()
        }
    }

    fun subscribe(subscription: Subscription, listener: IMqttActionListener?) {
        if (notConnected(true)) {
            return
        }
        try {
            mClient?.subscribe(subscription.topic, subscription.qos, null, listener)
        } catch (e: MqttException) {
            e.printStackTrace()
            Toast.makeText(this, "Failed to subscribe", Toast.LENGTH_SHORT).show()
        }
    }

    fun publish(publish: Publish, callback: IMqttActionListener?) {
        if (notConnected(true)) {
            return
        }
        try {
            mClient!!.publish(
                publish.topic,
                publish.payload.toByteArray(),
                publish.qos,
                publish.isRetained,
                null,
                callback
            )
        } catch (e: MqttException) {
            e.printStackTrace()
            Toast.makeText(this, "Failed to publish", Toast.LENGTH_SHORT).show()
        }
    }

    fun notConnected(showNotify: Boolean): Boolean {
        if (mClient == null || !mClient!!.isConnected) {
            if (showNotify) {
                Toast.makeText(this, "Client is not connected", Toast.LENGTH_SHORT).show()
            }
            return true
        }
        return false
    }

    override fun connectionLost(cause: Throwable?) {
        (mFragmentList[0] as ConnectionFragment).updateButtonText()
    }

    @Throws(Exception::class)
    override fun messageArrived(topic: String, message: MqttMessage) {
        (mFragmentList[3] as MessageFragment).updateMessage(Message(topic, message))
    }

    override fun deliveryComplete(token: IMqttDeliveryToken) {}
}