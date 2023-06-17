package info.mqtt.android.extsample.fragments

import android.content.Context
import android.os.Bundle
import android.view.*
import android.view.GestureDetector.SimpleOnGestureListener
import androidx.appcompat.app.ActionBarDrawerToggle
import androidx.appcompat.widget.Toolbar
import androidx.drawerlayout.widget.DrawerLayout
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import androidx.recyclerview.widget.RecyclerView.OnItemTouchListener
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.adapter.NavigationDrawerAdapter
import info.mqtt.android.extsample.databinding.FragmentNavigationDrawerBinding
import info.mqtt.android.extsample.internal.Connection
import info.mqtt.android.extsample.model.NavDrawerItem
import timber.log.Timber

class DrawerFragment : Fragment() {

    private val data: MutableList<NavDrawerItem> = ArrayList()
    private var drawerToggle: ActionBarDrawerToggle? = null
    private var drawerLayout: DrawerLayout? = null
    private lateinit var drawerAdapter: NavigationDrawerAdapter
    private var containerView: View? = null
    private var drawerListener: FragmentDrawerListener? = null

    private var _binding: FragmentNavigationDrawerBinding? = null

    // This property is only valid between onCreateView and onDestroyView.
    private val binding get() = _binding!!

    fun setDrawerListener(listener: FragmentDrawerListener?) {
        drawerListener = listener
    }

    fun addConnection(connection: Connection) {
        Timber.d("Adding new Connection: ${connection.id}")
        val navItem = NavDrawerItem(connection)
        data.add(navItem)
        drawerAdapter.notifyItemInserted(data.size - 1)
    }

    fun updateConnection(connection: Connection) {
        Timber.d("Updating Connection: ${connection.id}")
        val iterator: Iterator<NavDrawerItem> = data.iterator()
        var index = 0
        while (iterator.hasNext()) {
            var item = iterator.next()
            if (item.handle == connection.handle()) {
                item = NavDrawerItem(connection)
                data[index] = item
                break
            }
            index++
        }
        drawerAdapter.notifyDataSetChanged()
    }

    fun removeConnection(connection: Connection) {
        Timber.d("Removing connection from drawer: ${connection.id}")
        val iterator = data.iterator()
        while (iterator.hasNext()) {
            val item = iterator.next()
            if (item.handle == connection.handle()) {
                iterator.remove()
            }
        }
        drawerAdapter.notifyDataSetChanged()
    }

    fun clearConnections() {
        data.clear()
        drawerAdapter.notifyDataSetChanged()
    }

    fun notifyDataSetChanged() {
        drawerAdapter.notifyDataSetChanged()
    }

    private fun getData(): List<NavDrawerItem> {
        return data
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View {
        _binding = FragmentNavigationDrawerBinding.inflate(inflater, container, false)
        val view = binding.root

        binding.actionAddConnection.setOnClickListener {
            drawerListener!!.onAddConnectionSelected()
            drawerLayout!!.closeDrawer(containerView!!)
        }

        drawerAdapter = NavigationDrawerAdapter(requireContext(), getData())
        binding.drawerList.adapter = drawerAdapter
        binding.drawerList.layoutManager = LinearLayoutManager(activity)
        binding.drawerList.addOnItemTouchListener(RecyclerTouchListener(activity, binding.drawerList, object : ClickListener {
            override fun onClick(position: Int) {
                drawerListener!!.onDrawerItemSelected(position)
                drawerLayout!!.closeDrawer(containerView!!)
            }

            override fun onLongClick(position: Int) {
                Timber.d("I want to delete: $position")
                drawerListener!!.onDrawerItemLongSelected(position)
                drawerLayout!!.closeDrawer(containerView!!)
            }
        }))
        return view
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    fun setUp(fragmentId: Int, givenDrawerLayout: DrawerLayout, toolbar: Toolbar) {
        containerView = requireActivity().findViewById(fragmentId)
        drawerLayout = givenDrawerLayout
        drawerToggle = object : ActionBarDrawerToggle(activity, drawerLayout, toolbar, R.string.drawer_open, R.string.drawer_close) {
            override fun onDrawerOpened(drawerView: View) {
                super.onDrawerOpened(drawerView)
                activity!!.invalidateOptionsMenu()
            }

            override fun onDrawerClosed(drawerView: View) {
                super.onDrawerClosed(drawerView)
                activity!!.invalidateOptionsMenu()
            }

            override fun onDrawerSlide(drawerView: View, slideOffset: Float) {
                super.onDrawerSlide(drawerView, slideOffset)
                toolbar.alpha = 1 - slideOffset / 2
            }
        }
        drawerLayout!!.addDrawerListener(drawerToggle!!)
        drawerLayout!!.post { drawerToggle!!.syncState() }
    }

    interface ClickListener {
        fun onClick(position: Int)
        fun onLongClick(position: Int)
    }

    interface FragmentDrawerListener {
        fun onDrawerItemSelected(position: Int)
        fun onDrawerItemLongSelected(position: Int)
        fun onAddConnectionSelected()
    }

    internal class RecyclerTouchListener(context: Context?, recyclerView: RecyclerView, private val clickListener: ClickListener?) :
        OnItemTouchListener {
        private val gestureDetector: GestureDetector = GestureDetector(context, object : SimpleOnGestureListener() {
            override fun onSingleTapUp(e: MotionEvent): Boolean {
                return true
            }

            override fun onLongPress(e: MotionEvent) {
                val child = recyclerView.findChildViewUnder(e.x, e.y)
                if (child != null && clickListener != null) {
                    clickListener.onLongClick(recyclerView.getChildAdapterPosition(child))
                }
            }
        })

        override fun onInterceptTouchEvent(rv: RecyclerView, e: MotionEvent): Boolean {
            val child = rv.findChildViewUnder(e.x, e.y)
            if (child != null && clickListener != null && gestureDetector.onTouchEvent(e)) {
                clickListener.onClick(rv.getChildAdapterPosition(child))
            }
            return false
        }

        override fun onTouchEvent(rv: RecyclerView, e: MotionEvent) {}
        override fun onRequestDisallowInterceptTouchEvent(disallowIntercept: Boolean) {}

    }
}
