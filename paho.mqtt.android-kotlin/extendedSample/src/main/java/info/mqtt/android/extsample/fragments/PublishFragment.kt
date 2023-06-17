package info.mqtt.android.extsample.fragments

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.AdapterView
import android.widget.AdapterView.OnItemSelectedListener
import android.widget.ArrayAdapter
import android.widget.Toast
import androidx.fragment.app.Fragment
import info.mqtt.android.extsample.ActivityConstants
import info.mqtt.android.extsample.MainActivity
import info.mqtt.android.extsample.databinding.FragmentPublishBinding
import info.mqtt.android.extsample.internal.Connection
import info.mqtt.android.extsample.internal.Connections
import info.mqtt.android.service.QoS
import timber.log.Timber

class PublishFragment : Fragment() {

    private var connection: Connection? = null
    private var selectedQos = QoS.AtMostOnce
    private var retainValue = false

    private var _binding: FragmentPublishBinding? = null

    // This property is only valid between onCreateView and onDestroyView.
    private val binding get() = _binding!!

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val connections = Connections.getInstance(requireActivity()).connections
        connection = connections[requireArguments().getString(ActivityConstants.CONNECTION_KEY)]
        Timber.d("CONNECTION_KEY=${requireArguments().getString(ActivityConstants.CONNECTION_KEY)} '${connection!!.id}'")
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View {
        _binding = FragmentPublishBinding.inflate(inflater, container, false)
        val view = binding.root

        binding.topic.setText(DEFAULT_TOPIC)

        binding.qosSpinner.onItemSelectedListener = object : OnItemSelectedListener {

            override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                selectedQos = QoS.values()[position]
            }

            override fun onNothingSelected(parent: AdapterView<*>?) = Unit
        }
        binding.retainSwitch.setOnCheckedChangeListener { _, isChecked -> retainValue = isChecked }
        val adapter = ArrayAdapter(requireActivity(), android.R.layout.simple_spinner_dropdown_item, QoS.values())
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        binding.qosSpinner.adapter = adapter
        binding.publishButton.setOnClickListener {
            Timber.d("Publishing: [topic: ${binding.topic.text}, message: ${binding.message.text}, QoS: $selectedQos, Retain: $retainValue]")
            connection?.let { it1 ->
                (requireActivity() as MainActivity).publish(
                    it1,
                    binding.topic.text.toString(),
                    binding.message.text.toString(),
                    selectedQos,
                    retainValue
                )
            } ?: run {
                Toast.makeText(requireContext(), "Offline !", Toast.LENGTH_SHORT).show()
            }
        }
        return view
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    companion object {
        var DEFAULT_TOPIC = "/test"
    }
}