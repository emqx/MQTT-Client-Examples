package io.emqx.mqtt;

import org.eclipse.paho.client.mqttv3.*;

import java.nio.charset.StandardCharsets;
import java.text.MessageFormat;
import java.util.Arrays;

public class MqttTwoWayTlsSample {

    private static final String BROKER = "broker.emqx.io";
    private static final String PORT = "8883";
    private static final String CLIENT_ID = MqttClient.generateClientId();
    private static final String USERNAME = "emqx";
    private static final String PASSWORD = "public";
    private static final int CONNECT_TIMEOUT = 300;
    private static final boolean CLEAN_SESSION = true;
    private static final String TOPIC = "java-mqtt/tls";
    private static final int QoS = 1;
    private static final String PAYLOAD = "Enjoy the sample";

    private static final String CA_CERT_PATH = "path/to/cacert.crt";
    private static final String CLIENT_CERT_PATH = "path/to/client.crt";
    private static final String CLIENT_KEY_PATH = "path/to/client.key";

    public static void main(String[] args) {
        MqttClient client = null;
        try {
            String server = "ssl://" + BROKER + ":" + PORT;
            client = new MqttClient(server, CLIENT_ID);

            client.setCallback(new MqttCallback() {

                @Override
                public void connectionLost(Throwable cause) {
                    System.out.println(MessageFormat.format("Connection lost. Cause: {0}", cause));
                }

                @Override
                public void messageArrived(String topic, MqttMessage message) throws Exception {
                    System.out.println(MessageFormat.format("Callback: received message from topic {0}: {1}",
                            topic, message.toString()));
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    try {
                        System.out.println(MessageFormat.format("Callback: delivered message to topics {0}",
                                Arrays.asList(token.getTopics())));
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

            });

            MqttConnectOptions options = new MqttConnectOptions();
            options.setUserName(USERNAME);
            options.setPassword(PASSWORD.toCharArray());
            options.setConnectionTimeout(CONNECT_TIMEOUT);
            options.setCleanSession(CLEAN_SESSION);
            options.setSocketFactory(SSLUtils.getSocketFactory(CA_CERT_PATH, CLIENT_CERT_PATH, CLIENT_KEY_PATH, ""));

            System.out.println("Connecting to broker: " + server);
            client.connect(options);

            if (!client.isConnected()) {
                System.out.println("Failed to connect to broker: " + server);
                return;
            }
            System.out.println("Connected to broker: " + server);

            client.subscribe(TOPIC, QoS);
            System.out.println("Subscribed to topic: " + TOPIC);

            MqttMessage msg = new MqttMessage(PAYLOAD.getBytes(StandardCharsets.UTF_8));
            msg.setQos(QoS);
            client.publish(TOPIC, msg);

            System.out.println("Disconnect from broker: " + server);
            client.disconnect();
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            if (client != null) {
                try {
                    client.close();
                } catch (MqttException e) {
                    e.printStackTrace();
                }
            }
        }
    }

}
