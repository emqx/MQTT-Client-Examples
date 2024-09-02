import { useState } from "react";
import {
  SafeAreaView,
  ScrollView,
  View,
  Text,
  TextInput,
  StyleSheet,
  Button,
  StatusBar,
} from "react-native";
import mqtt from "mqtt";

export default function HomeScreen() {
  const [host, setHost] = useState("broker.emqx.io");
  const [username, setUsername] = useState("emqx");
  const [password, setPassword] = useState("test");
  const [connectBtnText, setConnectBtnText] = useState("Connect");
  const [subTopic, setSubTopic] = useState("/react_native");
  const [subBtnText, setSubBtnText] = useState("Subscribe");
  const [pubTopic, setPubTopic] = useState("/react_native");
  const [pubMessage, setPubMessage] = useState("Hello EMQX");
  const [receivedMsg, setReceivedMsg] = useState("");
  const [client, setClient] = useState<mqtt.MqttClient | null>(null);

  const protocol = "wss";
  const path = "/mqtt";
  const port = 8084;

  const doConnect = () => {
    try {
      setConnectBtnText("Connecting...");
      const clientId = `emqx-${new Date().getTime()}`;
      const newClient = mqtt.connect(`${protocol}://${host}:${port}${path}`, {
        clientId,
        username,
        password,
        reconnectPeriod: 1000,
        connectTimeout: 30 * 1000,
        // more options, refer to https://github.com/mqttjs/MQTT.js#mqttclientstreambuilder-options
      });

      setClient(newClient);

      newClient.on("connect", () => {
        setConnectBtnText("Connected");
      });
      newClient.on("error", (error) => {
        setConnectBtnText("Connect");
        console.log("onError", error);
      });
      newClient.on("message", (topic, payload) => {
        setReceivedMsg((prevMsg) => {
          return prevMsg.concat(`\n${payload.toString()}`);
        });
      });
      newClient.on("reconnect", () => {
        setConnectBtnText("Reconnecting...");
      });
      newClient.on("offline", () => {
        setConnectBtnText("Connect");
        console.log("onOffline");
      });
    } catch (error) {
      console.log("catch error:", error);
    }
  };
  const doSubscribe = () => {
    setSubBtnText("Subscribing...");
    client?.subscribe(subTopic, { qos: 0 }, (error) => {
      if (error) {
        console.error("Failed to subscribe to topic:", subTopic, error);
        setSubBtnText("Subscribe");
      } else {
        setSubBtnText("Subscribed");
        console.log("Subscribed to topic:", subTopic);
      }
    });
  };
  const doUnsubscribe = () => {
    client?.unsubscribe(subTopic, {}, (error) => {
      if (error) {
        console.error("Failed to unsubscribe from topic:", subTopic, error);
        setSubBtnText("Subscribe");
      } else {
        console.log("Unsubscribed from topic:", subTopic);
        setSubBtnText("Subscribe");
      }
    });
  };
  const doPublish = () => {
    client?.publish(pubTopic, pubMessage, { qos: 0 }, (error) => {
      if (error) {
        console.error("Failed to publish message:", error);
      } else {
        console.log("Message published to topic:", pubTopic);
      }
    });
  };
  const doDisconnect = () => {
    client?.end();
    setClient(null);
    setConnectBtnText("Connect");
  };

  return (
    <SafeAreaView style={styles.container}>
      <ScrollView contentContainerStyle={styles.scrollView}>
        <Text style={styles.label}>Host</Text>
        <TextInput
          autoCapitalize="none"
          style={styles.input}
          onChangeText={setHost}
          value={host}
        />
        <Text style={styles.label}>Username</Text>
        <TextInput
          autoCapitalize="none"
          style={styles.input}
          onChangeText={setUsername}
          value={username}
        />
        <Text style={styles.label}>Password</Text>
        <TextInput
          autoCapitalize="none"
          style={styles.input}
          onChangeText={setPassword}
          value={password}
          secureTextEntry={true}
        />
        <View style={styles.buttons}>
          <Button
            disabled={connectBtnText !== "Connect"}
            title={connectBtnText}
            onPress={doConnect}
          />
          <Button
            disabled={connectBtnText !== "Connected"}
            title="Disconnect"
            onPress={doDisconnect}
          />
        </View>

        <Text style={styles.label}>Subscribe Topic</Text>
        <TextInput
          style={styles.input}
          onChangeText={setSubTopic}
          value={subTopic}
        />
        <View style={styles.buttons}>
          <Button
            disabled={subBtnText !== "Subscribe"}
            title={subBtnText}
            onPress={doSubscribe}
          />
          <Button
            disabled={subBtnText !== "Subscribed"}
            title="Unsubscribe"
            onPress={doUnsubscribe}
          />
        </View>

        <Text style={styles.label}>Publish Topic</Text>
        <TextInput
          style={styles.input}
          onChangeText={setPubTopic}
          value={pubTopic}
        />
        <Text style={styles.label}>Publish Message</Text>
        <TextInput
          style={styles.input}
          onChangeText={setPubMessage}
          value={pubMessage}
        />
        <View style={styles.buttons}>
          <Button title="Publish" onPress={doPublish} />
        </View>
        <Text style={styles.label}>Received Message</Text>
        <Text style={styles.messages}>{receivedMsg}</Text>
      </ScrollView>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    paddingTop: StatusBar.currentHeight,
  },
  scrollView: {
    backgroundColor: "white",
    padding: 20,
  },
  label: {
    marginBottom: 8,
    color: "#1d1d1d",
    fontSize: 16,
  },
  input: {
    width: "100%",
    borderWidth: 1,
    borderColor: "#ccc",
    padding: 10,
    fontSize: 16,
    marginBottom: 12,
  },
  buttons: {
    flexDirection: "row",
    justifyContent: "space-between",
    marginBottom: 12,
  },
  messages: {
    fontSize: 16,
    borderWidth: 1,
    borderColor: "#ccc",
    paddingHorizontal: 10,
    minHeight: 160,
  },
});
