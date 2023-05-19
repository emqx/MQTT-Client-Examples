package io.emqx.mqtt;

import org.eclipse.paho.client.mqttv3.*;

import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManagerFactory;
import java.io.FileInputStream;
import java.security.*;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.text.MessageFormat;
import java.util.Arrays;

public class MqttOneWayTlsSample {
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
    private static final String CA_CERT_PATH = MqttOneWayTlsSample.class.getResource("").getPath()+"./broker.emqx.io-ca.crt";

    public static void main(String args[]) {
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
            options.setSocketFactory(getSocketFactory(CA_CERT_PATH));

            System.out.println("Connecting to broker: " + server);
            client.connect(options);

            if (!client.isConnected()) {
                System.out.println("Failed to connect to broker: " + server);
                return;
            }
            System.out.println("Connected to broker: " + server);

            client.subscribe(TOPIC, QoS);
            System.out.println("Subscribed to topic: " + TOPIC);

            MqttMessage msg = new MqttMessage(PAYLOAD.getBytes("UTF-8"));
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

    public static SSLSocketFactory getSocketFactory(String caCertPath) throws Exception {
        CertificateFactory certFactory = CertificateFactory.getInstance("X.509");

        // load CA certificate into keystore to authenticate server
        Certificate caCert = certFactory.generateCertificate(new FileInputStream(caCertPath));
        X509Certificate x509CaCert = (X509Certificate) caCert;

        KeyStore caKeyStore = KeyStore.getInstance(KeyStore.getDefaultType());
        caKeyStore.load(null, null);
        caKeyStore.setCertificateEntry("cacert", x509CaCert);

        TrustManagerFactory tmFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
        tmFactory.init(caKeyStore);

        SSLContext sslContext = SSLContext.getInstance("TLSv1.2");
        sslContext.init(null, tmFactory.getTrustManagers(), null);

        return sslContext.getSocketFactory();
    }
}
