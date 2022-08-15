package io.emqx.mqtt

import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.EditText
import android.widget.RadioButton
import android.widget.Toast
import org.eclipse.paho.client.mqttv3.IMqttActionListener
import org.eclipse.paho.client.mqttv3.IMqttToken
import org.eclipse.paho.client.mqttv3.MqttAsyncClient

class ConnectionFragment : BaseFragment() {
    private lateinit var mHost: EditText
    private lateinit var mPort: EditText
    private lateinit var mClientId: EditText
    private lateinit var mUsername: EditText
    private lateinit var mPassword: EditText
    private lateinit var mTlsButton: RadioButton
    private lateinit var mButton: Button
    override val layoutResId: Int
        get() = R.layout.fragment_connection

    override fun setUpView(view: View) {
        mHost = view.findViewById(R.id.host)
        mHost.setText("broker.emqx.io")
        mPort = view.findViewById(R.id.port)
        mClientId = view.findViewById(R.id.clientid)
        mClientId.setText(MqttAsyncClient.generateClientId())
        mUsername = view.findViewById(R.id.username)
        mPassword = view.findViewById(R.id.password)
        mTlsButton = view.findViewById(R.id.tls_true)
        mButton = view.findViewById(R.id.btn_connect)
        mButton.setOnClickListener {
            if (mButton.text.toString() == getString(R.string.connect)) {
                val connection = Connection(
                    fragmentActivity!!,
                    mHost.text.toString(),
                    mPort.text.toString().toInt(),
                    mClientId.text.toString(),
                    mUsername.text.toString(),
                    mPassword.text.toString(),
                    mTlsButton.isChecked
                )
                (fragmentActivity as MainActivity).connect(
                    connection,
                    object : IMqttActionListener {
                        override fun onSuccess(asyncActionToken: IMqttToken) {
                            Log.d(
                                "ConnectionFragment",
                                "Connected to: " + asyncActionToken.client.serverURI
                            )
                            updateButtonText()
                        }

                        override fun onFailure(asyncActionToken: IMqttToken, exception: Throwable) {
                            Toast.makeText(
                                fragmentActivity,
                                exception.toString(),
                                Toast.LENGTH_SHORT
                            ).show()
                            exception.printStackTrace()
                        }
                    })
            } else {
                (fragmentActivity as MainActivity).disconnect()
            }
        }
    }

    fun updateButtonText() {
        if ((fragmentActivity as MainActivity).notConnected(false)) {
            mButton.text = getText(R.string.connect)
        } else {
            mButton.text = getString(R.string.disconnect)
        }
    }

    companion object {
        fun newInstance(): ConnectionFragment {
            return ConnectionFragment()
        }
    }
}