package io.emqx.mqtt;

import com.sun.deploy.util.StringUtils;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.io.File;
import java.io.IOException;
import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class MqttFileTransferSample {

    /**
     * File segment size, default is 10KB, muse be less than `mqtt.max_packet_size` (default is 1024KB)
     */
    private final static int SEGMENT_SIZE = 1024 * 10;
    private final static String CLIENT_ID_PREFIX = "emqx-file-transfer-";
    private final static int QOS = 1;
    private MqttClient client;
    private String username;
    private String password;


    public MqttFileTransferSample(String username, String password) {
        this.username = username;
        this.password = password;
    }

    public void initClient(String brokerUrl, String clientId) {
        try {
            client = new MqttClient(brokerUrl, clientId, new MemoryPersistence());
            MqttConnectOptions options = new MqttConnectOptions();
            options.setUserName(username);
            options.setPassword(password.toCharArray());
            client.connect(options);
            if (!client.isConnected()) {
                log("Fail to connected " + brokerUrl + " with client ID " + client.getClientId());
                System.exit(1);
            }
            log("Connected " + brokerUrl + " with client ID " + client.getClientId());

        } catch (MqttException e) {
            e.printStackTrace();
            log(e.toString());
            System.exit(1);
        }

    }

    public void transferFile(String filePath, String host) {
        File file = new File(filePath);

        if (!file.exists()) {
            System.out.println("File does not exist : " + filePath);
            System.exit(1);
        }

        String clientId = CLIENT_ID_PREFIX + MqttClient.generateClientId();
        initClient(host, clientId);

        try {
            // Use file checksum as file_id
            String fileChecksum = calculateChecksum(filePath);

            String fileId = fileChecksum;
            long fileSize = file.length();

            // The client device publishes init command topic.
            // The payload of the message contains the file metadata, including the file name, size, and checksum.
            pubInitCommand(fileId, file.getName(), fileSize, fileChecksum);

            //  The client sends consecutive segment commands
            //  Each segment command carries a chunk of the file data at the specified offset.
            pubSegmentCommands(file, fileId);

            // The client sends finish command
            pubFinishCommand(fileId, fileSize);

        } catch (Exception e) {
            System.out.println("msg " + e.getMessage());
            System.out.println("log " + e.getLocalizedMessage());
            System.out.println("cause " + e.getCause());
            System.out.println("excep " + e);
            e.printStackTrace();
        }finally {
            try {
                client.disconnect();
                client.close();
            } catch (MqttException e) {
                throw new RuntimeException(e);
            }
        }

    }

    private void pubFinishCommand(String fileId, long fileSize) throws MqttException {
        String finishTopic = "$file/" + fileId + "/fin/" + fileSize;
        publishMessage(finishTopic, QOS, "".getBytes());
        log("File transfer finished.");
    }


    public void pubInitCommand(String fileId, String fileName, long fileSize, String fileChecksum) throws MqttException {
        String initTopic = "$file/" + fileId + "/init";

        String initMsg = initMsg(fileName, fileSize, fileChecksum);
        System.out.println("Init Msg : "+initMsg);
        publishMessage(initTopic, QOS, initMsg.getBytes());
        log("File transfer session initialized.");
    }

    public void pubSegmentCommands(File file, String fileId) throws IOException, MqttException {
        log("Send file segment start =>");
        FileChannel fileChannel = FileChannel.open(file.toPath());
        // Read the file and publish segments
        int offset = 0;
        while (true) {
            // Read a segment from the file
            int capacity = (int) file.length() - offset;
            capacity = Math.min(SEGMENT_SIZE, capacity);
            ByteBuffer buffer = ByteBuffer.allocate(capacity);
            fileChannel.read(buffer);
            buffer.flip();

            // Publish the segment
            String segmentTopic = "$file/" + fileId + "/" + offset;

            offset += buffer.limit();
            publishMessage(segmentTopic, QOS, buffer.array());

            // Check if the end of the file has been reached
            if (buffer.limit() < SEGMENT_SIZE) {
                break;
            }
        }
        fileChannel.close();
        log("Send file segment end ");
    }


    private static String initMsg(String fileName, long fileSize, String fileChecksum) {
        return "{\"name\":\"" + fileName + "\",\"size\":" + fileSize + ",\"checksum\":\"" + fileChecksum + "\"}";
    }


    public static String calculateChecksum(String filePath) throws IOException, NoSuchAlgorithmException {
        // Get the MessageDigest instance for the checksum algorithm
        MessageDigest md5 = MessageDigest.getInstance("SHA-256");

        // Get the file data
        File file = new File(filePath);

        byte[] fileData = Files.readAllBytes(Paths.get(file.getPath()));

        // Update the hash with the file data
        md5.update(fileData);

        // Convert the hash to a string
        return new BigInteger(1, md5.digest()).toString(16);
    }


    public void publishMessage(String topicName, int qos, byte[] payload) throws MqttException {
        if (!client.isConnected()) {
            System.out.println("client is unconnectd");
            System.exit(1);
        }

        MqttMessage message = new MqttMessage(payload);
        message.setQos(qos);
        client.publish(topicName, message);
        log("Published to topic \"" + topicName + "\" qos " + qos + " size:" + payload.length);
    }

    private static void log(String message) {
        System.out.println(message);
    }

    public static void main(String[] args) {
        String userName = "emqx";
        String password = "public";
        String broker = "tcp://broker.emqx.io:1883";
        String filePath = "";

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
                    case 'f':
                        filePath = args[++i];
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

        if(filePath == null || filePath.length() <=0){
            System.out.println("The argument f is required ");
            help();
            return;
        }

        System.out.println("Args => broker:"+broker+" filePath:"+filePath+" userName="+userName+" password:"+password);
        new MqttFileTransferSample(userName,password).transferFile(filePath, broker);
    }


    private static void help() {
        System.out.println(
                "Args:\n" +
                        "-h Help information\n" +
                        "-b MQTT broker url [default: tcp://broker.emqx.io:1883]\n" +
                        "-f The absolute path of the file to be uploaded [Required]\n" +
                        "-u Username [default: emqx]\n" +
                        "-z Password [default: public]\n"
        );
    }

}
