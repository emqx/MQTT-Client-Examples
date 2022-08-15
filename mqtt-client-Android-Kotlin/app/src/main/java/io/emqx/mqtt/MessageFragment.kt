package io.emqx.mqtt

import android.view.View
import androidx.recyclerview.widget.DividerItemDecoration
import androidx.recyclerview.widget.RecyclerView

class MessageFragment : BaseFragment() {
    private var mAdapter: MessageRecyclerViewAdapter? = null
    private val mList: ArrayList<Message> = ArrayList()


    override val layoutResId: Int
        get() = R.layout.fragment_message_list

    override fun setUpView(view: View) {
        val recyclerView = view.findViewById<RecyclerView>(R.id.message_list)
        recyclerView.addItemDecoration(
            DividerItemDecoration(
                fragmentActivity,
                DividerItemDecoration.VERTICAL
            )
        )
        mAdapter = MessageRecyclerViewAdapter(mList)
        recyclerView.adapter = mAdapter
    }

    fun updateMessage(message: Message) {
        mList.add(0, message)
        mAdapter!!.notifyItemInserted(0)
    }

    companion object {
        fun newInstance(): MessageFragment {
            return MessageFragment()
        }
    }
}