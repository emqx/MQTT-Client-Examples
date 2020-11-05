package io.emqx.mqtt;

import android.content.Context;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;

public class Connection {

    private String host;

    private int port;

    private String clientId;

    private String username;

    private String password;

    private boolean tls;

    private Context context;


    public Connection(Context context, String host, int port, String clientId, String username, String password, boolean tls) {
        this.context = context;
        this.host = host;
        this.port = port;
        this.clientId = clientId;
        this.username = username;
        this.password = password;
        this.tls = tls;
    }

    public MqttAndroidClient getMqttAndroidClient(Context context) {
        String uri;
        if (tls) {
            uri = "ssl://" + host + ":" + port;
        } else {
            uri = "tcp://" + host + ":" + port;
        }

        return new MqttAndroidClient(context, uri, clientId);

    }

    public MqttConnectOptions getMqttConnectOptions() {
        MqttConnectOptions options = new MqttConnectOptions();
        options.setCleanSession(false);
        if (tls) {
            try {
                options.setSocketFactory(SSLUtils.getSingleSocketFactory(context.getResources().openRawResource(R.raw.cacert)));
            } catch (Exception e) {
                e.printStackTrace();
            }
        }


        if (!getUsername().isEmpty()) {
            options.setUserName(getUsername());
        }

        if (!getPassword().isEmpty()) {
            options.setPassword(getPassword().toCharArray());
        }


        return options;
    }

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public String getClientId() {
        return clientId;
    }

    public void setClientId(String clientId) {
        this.clientId = clientId;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }
}
