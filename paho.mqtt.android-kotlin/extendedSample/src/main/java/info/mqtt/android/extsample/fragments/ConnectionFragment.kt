package info.mqtt.android.extsample.fragments

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.*
import androidx.appcompat.widget.SwitchCompat
import androidx.fragment.app.Fragment
import com.google.android.material.tabs.TabLayout
import info.mqtt.android.extsample.ActivityConstants
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.databinding.FragmentConnectionBinding
import info.mqtt.android.extsample.internal.Connection
import info.mqtt.android.extsample.internal.Connections.Companion.getInstance
import info.mqtt.android.extsample.utils.connect
import java.util.*

class ConnectionFragment : Fragment() {

    private var connection: Connection? = null
    private var connectSwitch: SwitchCompat? = null

    private var _binding: FragmentConnectionBinding? = null

    // This property is only valid between onCreateView and onDestroyView.
    private val binding get() = _binding!!

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val connections: HashMap<String, Connection> = getInstance(requireActivity()).connections
        connection = connections[requireArguments().getString(ActivityConstants.CONNECTION_KEY)]
        setHasOptionsMenu(true)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View {
        _binding = FragmentConnectionBinding.inflate(inflater, container, false)
        val view = binding.root

        binding.tablayout.addTab(binding.tablayout.newTab().setText("History").setId(0))
        binding.tablayout.addTab(binding.tablayout.newTab().setText("Messages").setId(1))
        binding.tablayout.addTab(binding.tablayout.newTab().setText("Publish").setId(2))
        binding.tablayout.addTab(binding.tablayout.newTab().setText("Subscribe").setId(3))
        binding.tablayout.addOnTabSelectedListener(object : TabLayout.OnTabSelectedListener {
            override fun onTabReselected(tab: TabLayout.Tab?) = Unit

            override fun onTabUnselected(tab: TabLayout.Tab?) = Unit

            override fun onTabSelected(tab: TabLayout.Tab?) {
                when (tab?.id) {
                    0 -> displayFragment(HistoryFragment())
                    1 -> displayFragment(MessagesFragment())
                    2 -> displayFragment(PublishFragment())
                    3 -> displayFragment(SubscriptionFragment())
                }
            }

        })

        return view
    }

    private fun changeConnectedState(state: Boolean) {
        binding.tablayout.getTabAt(1)?.view?.isClickable = state
        binding.tablayout.getTabAt(2)?.view?.isClickable = state
        connectSwitch?.isChecked = state
    }

    override fun onCreateOptionsMenu(menu: Menu, inflater: MenuInflater) {
        inflater.inflate(R.menu.menu_connection, menu)
        connectSwitch = menu.findItem(R.id.connect_switch).actionView?.findViewById(R.id.disConnectSwitch)
        connectSwitch?.setOnCheckedChangeListener { _, isChecked ->
            if (isChecked) {
                connection?.connect(requireActivity())
                changeConnectedState(true)
            } else {
                Handler(Looper.getMainLooper()).postDelayed(
                    { binding.tablayout.getTabAt(0)?.select() }, 100
                )
                connection?.client?.disconnect()
                changeConnectedState(false)
            }
        }
        changeConnectedState(connection!!.isConnected)
        super.onCreateOptionsMenu(menu, inflater)
    }

    private fun displayFragment(fragment: Fragment) {
        val bundle = Bundle()
        bundle.putString(ActivityConstants.CONNECTION_KEY, connection!!.handle())
        fragment.arguments = bundle
        val fragmentTransaction = parentFragmentManager.beginTransaction()
        fragmentTransaction.replace(R.id.tabcontent, fragment)
        fragmentTransaction.commit()
    }
}
