package io.emqx.mqtt;

import androidx.recyclerview.widget.RecyclerView;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;


import java.util.List;


public class SubscriptionRecyclerViewAdapter extends RecyclerView.Adapter<SubscriptionRecyclerViewAdapter.ViewHolder> {

    private final List<Subscription> mValues;

    public SubscriptionRecyclerViewAdapter(List<Subscription> items) {
        mValues = items;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.item_subscription, parent, false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(final ViewHolder holder, int position) {
        holder.mItem = mValues.get(position);
        holder.mTopicView.setText("Topic: " + mValues.get(position).getTopic());
        holder.mQosView.setText("Qos: "+mValues.get(position).getQos() + "");
    }

    @Override
    public int getItemCount() {
        return mValues.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder {
        public final View mView;
        public final TextView mTopicView;
        public final TextView mQosView;
        public Subscription mItem;

        public ViewHolder(View view) {
            super(view);
            mView = view;
            mTopicView = view.findViewById(R.id.tv_topic);
            mQosView = view.findViewById(R.id.tv_qos);
        }

        @Override
        public String toString() {
            return super.toString() + " '" + mQosView.getText() + "'";
        }
    }
}