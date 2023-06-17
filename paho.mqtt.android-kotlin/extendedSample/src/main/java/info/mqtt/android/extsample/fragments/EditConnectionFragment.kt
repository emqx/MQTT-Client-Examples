package info.mqtt.android.extsample.fragments

import android.annotation.SuppressLint
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.view.*
import android.widget.AdapterView
import android.widget.AdapterView.OnItemSelectedListener
import android.widget.ArrayAdapter
import androidx.fragment.app.Fragment
import info.mqtt.android.extsample.ActivityConstants
import info.mqtt.android.extsample.MainActivity
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.databinding.FragmentEditConnectionBinding
import info.mqtt.android.extsample.internal.Connection
import info.mqtt.android.extsample.internal.Connections.Companion.getInstance
import info.mqtt.android.extsample.model.ConnectionModel
import info.mqtt.android.service.QoS
import timber.log.Timber
import java.util.*

class EditConnectionFragment : Fragment() {
    private lateinit var formModel: ConnectionModel
    private var newConnection = true

    private var _binding: FragmentEditConnectionBinding? = null

    // This property is only valid between onCreateView and onDestroyView.
    private val binding get() = _binding!!

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setHasOptionsMenu(true)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View {
        _binding = FragmentEditConnectionBinding.inflate(inflater, container, false)
        val view = binding.root

        val adapter = ArrayAdapter(requireActivity(), android.R.layout.simple_spinner_dropdown_item, QoS.values())
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        if (this.arguments != null && requireArguments().getString(ActivityConstants.CONNECTION_KEY) != null) {
            val connections: HashMap<String, Connection> = getInstance(requireActivity()).connections
            val connectionKey = requireArguments().getString(ActivityConstants.CONNECTION_KEY)
            val connection = connections[connectionKey]
            Timber.d("Editing an existing connection: ${connection!!.handle()}")
            newConnection = false
            formModel = ConnectionModel(connection)
            Timber.d("Form Model: $formModel")
            formModel.clientHandle = connection.handle()
        } else {
            formModel = ConnectionModel()
        }
        populateFromConnectionModel(formModel)
        setFormItemListeners()

        return view
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private fun setFormItemListeners() {
        binding.clientId.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                formModel.clientId = s.toString()
            }
        })
        binding.hostname.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                formModel.serverHostName = s.toString()
            }
        })
        binding.addConnectionPort.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                if (s.toString().isNotEmpty()) {
                    formModel.serverPort = s.toString().toInt()
                }
            }
        })
        binding.cleanSessionSwitch.setOnCheckedChangeListener { _, isChecked -> formModel.isCleanSession = isChecked }
        binding.username.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                if (s.toString().trim { it <= ' ' } != "") {
                    formModel.username = s.toString()
                } else {
                    formModel.username = ActivityConstants.empty
                }
            }
        })
        binding.password.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                if (s.toString().trim { it <= ' ' } != "") {
                    formModel.password = s.toString()
                } else {
                    formModel.password = ActivityConstants.empty
                }
            }
        })
        binding.tlsServerKey.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                formModel.tlsServerKey = s.toString()
            }
        })
        binding.tlsClientKey.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                formModel.tlsClientKey = s.toString()
            }
        })
        binding.timeout.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                if (s.toString().isNotEmpty()) {
                    formModel.timeout = s.toString().toInt()
                }
            }
        })
        binding.keepalive.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                if (s.toString().isNotEmpty()) {
                    formModel.keepAlive = s.toString().toInt()
                }
            }
        })
        binding.lwtTopic.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                formModel.lwtTopic = s.toString()
            }
        })
        binding.lwtMessage.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable) {
                formModel.lwtMessage = s.toString()
            }
        })
        binding.lwtQosSpinner.onItemSelectedListener = object : OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>?, view: View, position: Int, id: Long) {
                formModel.lwtQos = QoS.values()[position]
            }

            override fun onNothingSelected(parent: AdapterView<*>?) {}
        }
        binding.retainSwitch.setOnCheckedChangeListener { _, isChecked -> formModel.isLwtRetain = isChecked }
    }

    @SuppressLint("SetTextI18n")
    private fun populateFromConnectionModel(connectionModel: ConnectionModel) {
        binding.clientId.setText(connectionModel.clientId)
        binding.hostname.setText(connectionModel.serverHostName)
        binding.addConnectionPort.setText(connectionModel.serverPort.toString())
        binding.cleanSessionSwitch.isChecked = connectionModel.isCleanSession
        binding.username.setText(connectionModel.username)
        binding.password.setText(connectionModel.password)
        binding.tlsServerKey.setText(connectionModel.tlsServerKey)
        binding.tlsClientKey.setText(connectionModel.tlsClientKey)
        binding.timeout.setText(connectionModel.timeout.toString())
        binding.keepalive.setText(connectionModel.keepAlive.toString())
        binding.lwtTopic.setText(connectionModel.lwtTopic)
        binding.lwtMessage.setText(connectionModel.lwtMessage)
        binding.lwtQosSpinner.setSelection(connectionModel.lwtQos.value)
        binding.retainSwitch.isChecked = connectionModel.isLwtRetain
    }

    private fun saveConnection() {
        Timber.d("newConnection=$newConnection $formModel")
        if (newConnection) {
            // Generate a new Client Handle
            val sb = StringBuilder(length)
            for (i in 0 until length) {
                sb.append(AB[random.nextInt(AB.length)])
            }
            val clientHandle = sb.toString() + '-' + formModel.serverHostName + '-' + formModel.clientId
            formModel.clientHandle = clientHandle
            (activity as MainActivity).persistAndConnect(formModel)
        } else {
            // Update an existing connection
            (activity as MainActivity).updateAndConnect(formModel)
        }
    }

    override fun onCreateOptionsMenu(menu: Menu, inflater: MenuInflater) {
        // Inflate the menu; this adds items to the action bar if it is present.
        inflater.inflate(R.menu.menu_edit_connection, menu)
        super.onCreateOptionsMenu(menu, inflater)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        val id = item.itemId
        if (id == R.id.action_save_connection) {
            saveConnection()
        }
        return super.onOptionsItemSelected(item)
    }

    companion object {
        private const val AB = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        private val random = Random()
        private const val length = 8
    }
}
