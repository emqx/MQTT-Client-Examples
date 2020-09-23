package io.emqx.mqtt;

import android.os.Bundle;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.viewpager.widget.ViewPager;

import com.google.android.material.tabs.TabLayout;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements MqttCallback {

    private MqttAndroidClient mClient;

    private List<Fragment> mFragmentList = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mFragmentList.add(ConnectionFragment.newInstance());
        mFragmentList.add(SubscriptionFragment.newInstance());
        mFragmentList.add(PublishFragment.newInstance());
        mFragmentList.add(MessageFragment.newInstance());
        SectionsPagerAdapter sectionsPagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager(), this, mFragmentList);
        ViewPager viewPager = findViewById(R.id.view_pager);
        viewPager.setOffscreenPageLimit(3);
        viewPager.setAdapter(sectionsPagerAdapter);
        TabLayout tabs = findViewById(R.id.tabs);
        tabs.setupWithViewPager(viewPager);

    }

    public void connect(Connection connection, IMqttActionListener listener) {
        mClient = connection.getMqttAndroidClient(this);
        try {
            mClient.connect(connection.getMqttConnectOptions(), null, listener);
            mClient.setCallback(this);
        } catch (MqttException e) {
            e.printStackTrace();
            Toast.makeText(this, "Failed to connect", Toast.LENGTH_SHORT).show();
        }

    }

    public void disconnect() {
        if (notConnected(true)) {
            return;
        }
        try {
            mClient.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void subscribe(Subscription subscription, IMqttActionListener listener) {
        if (notConnected(true)) {
            return;
        }
        try {
            mClient.subscribe(subscription.getTopic(), subscription.getQos(), null, listener);
        } catch (MqttException e) {
            e.printStackTrace();
            Toast.makeText(this, "Failed to subscribe", Toast.LENGTH_SHORT).show();
        }
    }

    public void publish(Publish publish, IMqttActionListener callback) {
        if (notConnected(true)) {
            return;
        }
        try {
            mClient.publish(publish.getTopic(), publish.getPayload().getBytes(), publish.getQos(), publish.isRetained(), null, callback);
        } catch (MqttException e) {
            e.printStackTrace();
            Toast.makeText(this, "Failed to publish", Toast.LENGTH_SHORT).show();
        }
    }

    public boolean notConnected(boolean showNotify) {
        if (mClient == null || !mClient.isConnected()) {
            if (showNotify) {
                Toast.makeText(this, "Client is not connected", Toast.LENGTH_SHORT).show();
            }
            return true;
        }
        return false;
    }

    @Override
    public void connectionLost(Throwable cause) {
        ((ConnectionFragment) mFragmentList.get(0)).updateButtonText();
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        ((MessageFragment) mFragmentList.get(3)).updateMessage(new Message(topic, message));

    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {

    }
}