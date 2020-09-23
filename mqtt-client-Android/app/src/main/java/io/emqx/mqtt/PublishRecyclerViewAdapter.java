package io.emqx.mqtt;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

public class PublishRecyclerViewAdapter extends RecyclerView.Adapter<PublishRecyclerViewAdapter.ViewHolder> {

    private final List<Publish> mValues;

    public PublishRecyclerViewAdapter(List<Publish> items) {
        mValues = items;
    }

    @Override
    public PublishRecyclerViewAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.item_publish, parent, false);
        return new PublishRecyclerViewAdapter.ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(final PublishRecyclerViewAdapter.ViewHolder holder, int position) {
        Publish publish = mValues.get(position);
        holder.mItem = publish;
        holder.mTopicView.setText("Topic: " + publish.getTopic() + "  Qos: " + publish.getQos() + "  Retained: " + publish.isRetained());
        holder.mPayloadView.setText("Payload: " + publish.getPayload());
    }

    @Override
    public int getItemCount() {
        return mValues.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder {
        public final View mView;
        public final TextView mTopicView;
        public final TextView mPayloadView;
        public Publish mItem;

        public ViewHolder(View view) {
            super(view);
            mView = view;
            mTopicView = view.findViewById(R.id.textView);
            mPayloadView = view.findViewById(R.id.payload);
        }

    }
}
