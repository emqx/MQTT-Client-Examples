package io.emqx.mqtt;

import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;

import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttAsyncClient;

public class ConnectionFragment extends BaseFragment {


    private EditText mHost;
    private EditText mPort;
    private EditText mClientId;
    private EditText mUsername;
    private EditText mPassword;
    private RadioButton mTlsButton;
    private Button mButton;

    public ConnectionFragment() {
    }

    public static ConnectionFragment newInstance() {
        return new ConnectionFragment();
    }

    @Override
    protected int getLayoutResId() {
        return R.layout.fragment_connection;
    }

    @Override
    protected void setUpView(View view) {
        mHost = view.findViewById(R.id.host);
        mHost.setText("broker.emqx.io");
        mPort = view.findViewById(R.id.port);
        mClientId = view.findViewById(R.id.clientid);
        mClientId.setText(MqttAsyncClient.generateClientId());
        mUsername = view.findViewById(R.id.username);
        mPassword = view.findViewById(R.id.password);
        mTlsButton = view.findViewById(R.id.tls_true);
        mButton = view.findViewById(R.id.btn_connect);
        mButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mButton.getText().toString().equals(getString(R.string.connect))) {
                    Connection connection = new Connection(mHost.getText().toString(), Integer.parseInt(mPort.getText().toString()), mClientId.toString(), mUsername.toString(), mPassword.toString(), mTlsButton.isChecked());
                    ((MainActivity) fragmentActivity).connect(connection, new IMqttActionListener() {
                        @Override
                        public void onSuccess(IMqttToken asyncActionToken) {
                            updateButtonText();
                        }

                        @Override
                        public void onFailure(IMqttToken asyncActionToken, Throwable exception) {

                        }
                    });
                } else {
                    ((MainActivity) fragmentActivity).disconnect();
                }

            }
        });
    }

    public void updateButtonText() {
        if (((MainActivity) fragmentActivity).notConnected(false)) {
            mButton.setText(getText(R.string.connect));
        } else {
            mButton.setText(getString(R.string.disconnect));
        }
    }


}