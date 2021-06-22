package io.emqx.mqtt;

import org.eclipse.paho.client.mqttv3.*;

import java.io.IOException;

public class MqttExample implements MqttCallback {

    public static void main(String[] args) {
        String broker = "tcp://broker.emqx.io:1883";
        int qos = 0;
        String action = "publish";
        String topic = "test/topic";
        String message = "Hello MQTT";
        String clientId = MqttClient.generateClientId();
        boolean cleanSession = true;
        String userName = "emqx";
        String password = "public";
        for (int i = 0; i < args.length; i++) {
            if (args[i].length() == 2 && args[i].startsWith("-")) {
                char arg = args[i].charAt(1);
                if (arg == 'h') {
                    help();
                    return;
                }

                if (i == args.length - 1 || args[i + 1].charAt(0) == '-') {
                    System.out.println("Missing value for argument: " + args[i]);
                    help();
                    return;
                }
                switch (arg) {
                    case 'b':
                        broker = args[++i];
                        break;
                    case 'a':
                        action = args[++i];
                        break;
                    case 't':
                        topic = args[++i];
                        break;
                    case 'q':
                        qos = Integer.parseInt(args[++i]);
                        break;
                    case 'c':
                        cleanSession = Boolean.parseBoolean(args[++i]);
                        break;
                    case 'u':
                        userName = args[++i];
                        break;
                    case 'z':
                        password = args[++i];
                        break;
                    default:
                        System.out.println("Unknown argument: " + args[i]);
                        help();
                        return;
                }
            } else {
                System.out.println("Unknown argument: " + args[i]);
                help();
                return;
            }
        }

        if (!action.equals("publish") && !action.equals("subscribe")) {
            System.out.println("Invalid action: " + action);
            help();
            return;
        }
        if (qos < 0 || qos > 2) {
            System.out.println("Invalid QoS: " + qos);
            help();
            return;
        }

        MqttExample sample = new MqttExample(broker, clientId, cleanSession, userName, password);
        try {
            if (action.equals("publish")) {
                sample.publish(topic, qos, message.getBytes());
            } else {
                sample.subscribe(topic, qos);
            }
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    private MqttClient client;
    private String brokerUrl;
    private MqttConnectOptions options;
    private boolean clean;
    private String password;
    private String userName;

    public MqttExample(String brokerUrl, String clientId, boolean cleanSession, String userName, String password) {
        this.brokerUrl = brokerUrl;
        this.clean = cleanSession;
        this.password = password;
        this.userName = userName;

        options = new MqttConnectOptions();
        options.setCleanSession(clean);
        if (userName != null) {
            options.setUserName(this.userName);
        }
        if (password != null) {
            options.setPassword(this.password.toCharArray());
        }

        try {
            client = new MqttClient(this.brokerUrl, clientId);
            client.setCallback(this);
        } catch (MqttException e) {
            e.printStackTrace();
            log(e.toString());
            System.exit(1);
        }
    }

    public void subscribe(String topicName, int qos) throws MqttException {

        client.connect(options);
        log("Connected to " + brokerUrl + " with client ID " + client.getClientId());

        client.subscribe(topicName, qos);
        log("Subscribed to topic: " + topicName + " qos " + qos);

        try {
            System.in.read();
        } catch (IOException e) {
        }

        client.disconnect();
        log("Disconnected");
    }

    public void publish(String topicName, int qos, byte[] payload) throws MqttException {
        client.connect(options);
        log("Connected to " + brokerUrl + " with client ID " + client.getClientId());

        MqttMessage message = new MqttMessage(payload);
        message.setQos(qos);

        client.publish(topicName, message);
        log("Published to topic \"" + topicName + "\" qos " + qos);
        client.disconnect();
        log("Disconnected");
    }

    private void log(String message) {
        System.out.println(message);
    }


    public void connectionLost(Throwable throwable) {
        log("Connection lost: " + throwable);
        System.exit(1);
    }

    public void messageArrived(String s, MqttMessage mqttMessage) throws Exception {
        log("Received message:\n" +
                "Topic: " + s + "\t" +
                "Message: " + mqttMessage.toString()
        );
    }

    public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {


    }

    private static void help() {
        System.out.println(
                "Args:\n" +
                        "-h Help information\n" +
                        "-b MQTT broker url [default: tcp://broker.emqx.io:1883]\n" +
                        "-a publish/subscribe action [default: publish]\n" +
                        "-u Username [default: emqx]\n" +
                        "-z Password [default: public]\n" +
                        "-c Clean session [default: true]\n" +
                        "-t Publish/Subscribe topic [default: test/topic]\n" +
                        "-q QoS [default: 0]"
        );
    }
}
