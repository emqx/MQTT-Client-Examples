package info.mqtt.android.extsample.fragments

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import info.mqtt.android.extsample.ActivityConstants
import info.mqtt.android.extsample.adapter.HistoryListItemAdapter
import info.mqtt.android.extsample.databinding.FragmentConnectionHistoryBinding
import info.mqtt.android.extsample.internal.Connection
import info.mqtt.android.extsample.internal.Connections
import timber.log.Timber

class HistoryFragment : Fragment() {

    private lateinit var historyListItemAdapter: HistoryListItemAdapter
    private lateinit var connection: Connection

    private var _binding: FragmentConnectionHistoryBinding? = null

    // This property is only valid between onCreateView and onDestroyView.
    private val binding get() = _binding!!

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val connections = Connections.getInstance(requireActivity()).connections
        connection = connections[requireArguments().getString(ActivityConstants.CONNECTION_KEY)]!!
        setHasOptionsMenu(true)
        Timber.d("CONNECTION_KEY=${requireArguments().getString(ActivityConstants.CONNECTION_KEY)} '${connection.id}'")
        setHasOptionsMenu(true)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View {
        _binding = FragmentConnectionHistoryBinding.inflate(inflater, container, false)
        historyListItemAdapter = HistoryListItemAdapter(requireContext(), connection.history.value!!)
        binding.historyListView.adapter = historyListItemAdapter
        binding.historyClearButton.setOnClickListener {
            connection.historyList.clear()
            historyListItemAdapter.notifyDataSetChanged()
        }

        connection.history.observe(this.viewLifecycleOwner, {
            historyListItemAdapter.notifyDataSetChanged()
        })

        return binding.root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
