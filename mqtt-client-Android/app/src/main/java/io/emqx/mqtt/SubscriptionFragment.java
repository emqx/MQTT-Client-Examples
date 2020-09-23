package io.emqx.mqtt;

import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.Toast;

import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.RecyclerView;

import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;

import java.util.ArrayList;
import java.util.List;


public class SubscriptionFragment extends BaseFragment {


    private EditText mTopic;
    private RadioGroup mRadioGroup;

    private SubscriptionRecyclerViewAdapter mAdapter;

    private List<Subscription> mSubscriptionList = new ArrayList<>();


    public static SubscriptionFragment newInstance() {
        return new SubscriptionFragment();
    }

    @Override
    protected int getLayoutResId() {
        return R.layout.fragment_subscription_list;
    }

    @Override
    protected void setUpView(View view) {
        RecyclerView recyclerView = view.findViewById(R.id.subscription_list);
        mAdapter = new SubscriptionRecyclerViewAdapter(mSubscriptionList);
        recyclerView.setAdapter(mAdapter);
        recyclerView.addItemDecoration(new DividerItemDecoration(fragmentActivity, DividerItemDecoration.VERTICAL));
        mTopic = view.findViewById(R.id.topic);
        mRadioGroup = view.findViewById(R.id.qos);
        Button subBtn = view.findViewById(R.id.subscribe);
        subBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final Subscription subscription = getSubscription();
                final String topic = mTopic.getText().toString();
                ((MainActivity) fragmentActivity).subscribe(subscription, new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        mSubscriptionList.add(0, subscription);
                        mAdapter.notifyItemInserted(0);
                    }

                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        Toast.makeText(fragmentActivity, "Failed to subscribe", Toast.LENGTH_SHORT).show();

                    }
                });

            }
        });
    }


    private Subscription getSubscription() {
        String topic = mTopic.getText().toString();
        int qos = 0;
        int qosCheckedId = mRadioGroup.getCheckedRadioButtonId();
        switch (qosCheckedId) {
            case R.id.qos0:
                qos = 0;
                break;
            case R.id.qos1:
                qos = 1;
                break;
            case R.id.qos2:
                qos = 2;
                break;
        }


        return new Subscription(topic, qos);
    }


}