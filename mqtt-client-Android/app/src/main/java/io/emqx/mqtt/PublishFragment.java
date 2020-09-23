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

public class PublishFragment extends BaseFragment {

    private EditText mTopic;
    private EditText mPayload;
    private RadioGroup mQosRadioGroup;
    private RadioGroup mRetainedRadioGroup;


    PublishRecyclerViewAdapter mAdapter;

    List<Publish> mPublishList = new ArrayList<>();


    public static PublishFragment newInstance() {
        return new PublishFragment();
    }

    @Override
    protected int getLayoutResId() {
        return R.layout.fragment_publish_list;
    }

    @Override
    protected void setUpView(View view) {
        RecyclerView recyclerView = view.findViewById(R.id.publication_list);
        mAdapter = new PublishRecyclerViewAdapter(mPublishList);
        recyclerView.setAdapter(mAdapter);
        recyclerView.addItemDecoration(new DividerItemDecoration(fragmentActivity, DividerItemDecoration.VERTICAL));
        mTopic = view.findViewById(R.id.topic);
        mPayload = view.findViewById(R.id.payload);
        mQosRadioGroup = view.findViewById(R.id.qos);
        mRetainedRadioGroup = view.findViewById(R.id.retained);

        Button pubBtn = view.findViewById(R.id.publish);
        pubBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final Publish publish = getPublish();
                ((MainActivity) fragmentActivity).publish(publish, new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        mPublishList.add(0, publish);
                        mAdapter.notifyItemInserted(0);
                    }

                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        Toast.makeText(fragmentActivity, "Failed to publish", Toast.LENGTH_SHORT).show();

                    }
                });

            }
        });

    }


    private Publish getPublish() {
        String topic = mTopic.getText().toString();
        String message = mPayload.getText().toString();
        int qos = 0;
        int qosCheckedId = mQosRadioGroup.getCheckedRadioButtonId();
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
        boolean retained = false;
        int retainedCheckedId = mRetainedRadioGroup.getCheckedRadioButtonId();
        switch (retainedCheckedId) {
            case R.id.retained_true:
                retained = true;
                break;
            case R.id.retained_false:
                retained = false;
                break;

        }

        return new Publish(topic, message, qos, retained);
    }
}
