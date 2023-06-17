package info.mqtt.android.extsample.fragments

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import info.mqtt.android.extsample.ActivityConstants
import info.mqtt.android.extsample.MainActivity
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.databinding.FragmentManageBinding
import info.mqtt.android.extsample.internal.Connection
import info.mqtt.android.extsample.internal.Connections
import timber.log.Timber

class ManageConnectionFragment : Fragment() {
    private var connection: Connection? = null
    private lateinit var connections: HashMap<String, Connection>
    private var connectionKey: String? = null

    private var _binding: FragmentManageBinding? = null

    // This property is only valid between onCreateView and onDestroyView.
    private val binding get() = _binding!!

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        connections = Connections.getInstance(requireActivity()).connections
        connectionKey = requireArguments().getString(ActivityConstants.CONNECTION_KEY)
        connection = connections[connectionKey]
        setHasOptionsMenu(false)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View {
        _binding = FragmentManageBinding.inflate(inflater, container, false)
        val view = binding.root

        val name = connection!!.id + "@" + connection!!.hostName + ":" + connection!!.port
        binding.connectionIdText.text = name
        binding.deleteButton.setOnClickListener {
            Timber.d("Deleting Connection: $name.")
            connections.remove(connectionKey)
            Connections.getInstance(requireActivity()).removeConnection(connection!!)
            val fragmentTransaction = parentFragmentManager.beginTransaction()
            fragmentTransaction.replace(R.id.container_body, HomeFragment())
            fragmentTransaction.commit()
            (activity as MainActivity?)!!.removeConnectionRow(connection)
        }
        binding.editButton.setOnClickListener {
            Timber.d("Editing Connection: $name.")
            val editConnectionFragment = EditConnectionFragment()
            val bundle = Bundle()
            bundle.putString(ActivityConstants.CONNECTION_KEY, connection!!.handle())
            editConnectionFragment.arguments = bundle
            val fragmentTransaction = parentFragmentManager.beginTransaction()
            fragmentTransaction.replace(R.id.container_body, editConnectionFragment)
            fragmentTransaction.commit()
        }

        return view
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

}
