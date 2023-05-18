package io.emqx.mqtt;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;


public class MqttSample {
    public static void main(String[] args) {
        String topic = "test/topic";
        String content = "Hello World";
        int qos = 2;
        String broker = "tcp://broker.emqx.io:1883";
        String clientId = MqttClient.generateClientId();
        MemoryPersistence persistence = new MemoryPersistence();
        MqttConnectOptions connOpts = new MqttConnectOptions();
        connOpts.setUserName("emqx_user");
        connOpts.setPassword("emqx_password".toCharArray());

        // ssl/tls config
        try {
            // broker = "ssl://broker.emqx.io:8883";

            // one-way ssl/tls
            // String caCrtFile = MqttSample.class.getResource("").getPath() + "./broker.emqx.io-ca.crt";
            // connOpts.setSocketFactory(SSLUtils.getSingleSocketFactory(caCrtFile));

            // two-way ssl/tls
            // String caCrtFile = MqttSample.class.getResource("").getPath() + "./server-ca.crt";
            // String crtFile = MqttSample.class.getResource("").getPath() + "./client.crt";
            // String keyFile = MqttSample.class.getResource("").getPath() + "./client.key";
            // connOpts.setSocketFactory(SSLUtils.getSocketFactory(caCrtFile, crtFile, keyFile, ""));
        } catch (Exception e) {
            throw new RuntimeException(e);
        }

        try {
            MqttClient client = new MqttClient(broker, clientId, persistence);
            client.setCallback(new SampleCallback());

            System.out.println("Connecting to broker: " + broker);
            client.connect(connOpts);
            System.out.println("Connected to broker: " + broker);
            client.subscribe(topic, qos);
            System.out.println("Subscribed to topic: " + topic);
            MqttMessage message = new MqttMessage(content.getBytes());
            message.setQos(qos);
            client.publish(topic, message);
            System.out.println("Message published");
            client.disconnect();
            System.out.println("Disconnected");
            client.close();
            System.exit(0);
        } catch (MqttException me) {
            System.out.println("reason " + me.getReasonCode());
            System.out.println("msg " + me.getMessage());
            System.out.println("loc " + me.getLocalizedMessage());
            System.out.println("cause " + me.getCause());
            System.out.println("excep " + me);
            me.printStackTrace();
        }
    }

}
