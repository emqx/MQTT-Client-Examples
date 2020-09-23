package io.emqx.mqtt;

import android.view.View;

import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.List;


public class MessageFragment extends BaseFragment {


    private MessageRecyclerViewAdapter mAdapter;

    private List<Message> mList = new ArrayList<>();

    public MessageFragment() {
    }


    public static MessageFragment newInstance() {
        return new MessageFragment();
    }

    @Override
    protected int getLayoutResId() {
        return R.layout.fragment_message_list;
    }

    @Override
    protected void setUpView(View view) {
        RecyclerView recyclerView = view.findViewById(R.id.message_list);
        recyclerView.addItemDecoration(new DividerItemDecoration(fragmentActivity, DividerItemDecoration.VERTICAL));
        mAdapter = new MessageRecyclerViewAdapter(mList);
        recyclerView.setAdapter(mAdapter);

    }


    public void updateMessage(Message message) {
        mList.add(0, message);
        mAdapter.notifyItemInserted(0);

    }


}