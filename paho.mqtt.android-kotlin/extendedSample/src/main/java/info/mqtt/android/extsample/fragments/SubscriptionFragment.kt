package info.mqtt.android.extsample.fragments

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import android.widget.AdapterView
import android.widget.AdapterView.OnItemSelectedListener
import android.widget.ArrayAdapter
import androidx.appcompat.app.AlertDialog
import androidx.fragment.app.Fragment
import info.mqtt.android.extsample.ActivityConstants
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.adapter.SubscriptionListItemAdapter
import info.mqtt.android.extsample.databinding.FragmentSubscriptionsBinding
import info.mqtt.android.extsample.databinding.SubscriptionDialogBinding
import info.mqtt.android.extsample.internal.Connection
import info.mqtt.android.extsample.internal.Connections.Companion.getInstance
import info.mqtt.android.extsample.model.Subscription
import info.mqtt.android.service.QoS
import org.eclipse.paho.client.mqttv3.MqttException
import timber.log.Timber
import java.util.*


class SubscriptionFragment : Fragment() {

    private var tempQosValue = QoS.AtMostOnce
    private lateinit var connection: Connection

    private var _binding: FragmentSubscriptionsBinding? = null

    // This property is only valid between onCreateView and onDestroyView.
    private val binding get() = _binding!!

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val bundle = this.arguments
        val connectionHandle = bundle!!.getString(ActivityConstants.CONNECTION_KEY)
        val connections: HashMap<String, Connection> = getInstance(requireActivity()).connections
        connection = connections[connectionHandle]!!
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View {
        _binding = FragmentSubscriptionsBinding.inflate(inflater, container, false)
        val rootView = binding.root
        binding.subscribeButton.setOnClickListener { showInputDialog() }
        binding.subscriptionListView.adapter = SubscriptionListItemAdapter(requireContext(), connection)
        return rootView
    }

    private fun showInputDialog() {
        val dialogBinding = SubscriptionDialogBinding.inflate(LayoutInflater.from(context))

        val adapter = ArrayAdapter(requireActivity(), android.R.layout.simple_spinner_dropdown_item, QoS.values())
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        dialogBinding.subscriptionQosSpinner.adapter = adapter
        dialogBinding.subscriptionQosSpinner.onItemSelectedListener = object : OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>?, view: View, position: Int, id: Long) {
                tempQosValue = QoS.values()[position]
            }

            override fun onNothingSelected(parent: AdapterView<*>?) {}
        }
        val alertDialogBuilder = AlertDialog.Builder(requireActivity())
        alertDialogBuilder.setView(dialogBinding.root)
        alertDialogBuilder.setCancelable(true).setPositiveButton(R.string.subscribe_ok) { _, _ ->
            val topic = dialogBinding.subscriptionTopicEditText.text.toString()
            val subscription = Subscription(topic, tempQosValue, connection.handle(), dialogBinding.showNotificationsSwitch.isChecked)
            try {
                connection.addNewSubscription(subscription)
                (binding.subscriptionListView.adapter as SubscriptionListItemAdapter).refresh()
            } catch (ex: MqttException) {
                Timber.d(ex)
            }
            adapter.notifyDataSetChanged()
        }.setNegativeButton(R.string.subscribe_cancel) { dialog, _ -> dialog.cancel() }
        val alert = alertDialogBuilder.create()
        alert.window!!.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_VISIBLE)
        alert.show()
    }
}