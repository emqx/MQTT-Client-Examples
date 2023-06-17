package info.mqtt.android.extsample.adapter

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.core.content.ContextCompat
import androidx.recyclerview.widget.RecyclerView
import info.mqtt.android.extsample.R
import info.mqtt.android.extsample.adapter.NavigationDrawerAdapter.MyViewHolder
import info.mqtt.android.extsample.model.NavDrawerItem

class NavigationDrawerAdapter(private val context: Context, data: List<NavDrawerItem>) : RecyclerView.Adapter<MyViewHolder>() {

    private val inflater: LayoutInflater = LayoutInflater.from(context)
    private var data: List<NavDrawerItem> = emptyList()

    init {
        this.data = data
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyViewHolder {
        val view = inflater.inflate(R.layout.nav_drawer_row, parent, false)
        return MyViewHolder(view)
    }

    override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
        val current = data[position]
        holder.navTitle.text = current.title
        val doneCloud = ContextCompat.getDrawable(context, R.drawable.ic_cloud_done_dark)
        holder.icon.setImageDrawable(doneCloud)
    }

    override fun getItemCount(): Int {
        return data.size
    }

    inner class MyViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        val navTitle: TextView = itemView.findViewById(R.id.navTitle)
        val icon: ImageView = itemView.findViewById(R.id.connection_icon)
    }

}
