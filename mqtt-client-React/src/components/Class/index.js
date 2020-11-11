import React, { createContext } from "react";
import Connection from "./Connection";
import Publisher from "./Publisher";
import Subscriber from "./Subscriber";
import Receiver from "./Receiver";
import mqtt from "mqtt";

export const QosOption = createContext([]);
const qosOption = [
  {
    label: "0",
    value: 0,
  },
  {
    label: "1",
    value: 1,
  },
  {
    label: "2",
    value: 2,
  },
];

class ClassMqtt extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      client: null,
      connectStatus: "Connect",
      isSubed: false,
      messages: [],
    };
  }

  handleConnect = (host, mqttOptions) => {
    this.setState({ connectStatus: "Connecting" });
    this.client = mqtt.connect(host, mqttOptions);

    if (this.client) {
      this.client.on("connect", () => {
        this.setState({ connectStatus: "Connected" });
      });
      this.client.on("error", (err) => {
        console.error("Connection error: ", err);
        this.client.end();
      });
      this.client.on("reconnect", () => {
        this.setState({ connectStatus: "Reconnecting" });
      });
      this.client.on("message", (topic, message) => {
        const payload = { topic, message: message.toString() };
        const { messages } = this.state;
        if (payload.topic) {
          const changedMessages = messages.concat([payload]);
          this.setState({ messages: changedMessages });
        }
      });
    }
  };

  handleSubscribe = (topic, qos) => {
    if (this.client) {
      this.client.subscribe(topic, { qos }, (error) => {
        if (error) {
          console.log("Subscribe to topics error", error);
          return;
        }
        this.setState({ isSubed: true });
      });
    }
  };

  handleUnsub = (topic) => {
    if (this.client) {
      this.client.unsubscribe(topic, (error) => {
        if (error) {
          console.log("Unsubscribe error", error);
          return;
        }
        this.setState({ isSubed: false });
      });
    }
  };

  handlePublish = (pubRecord) => {
    if (this.client) {
      const { topic, qos, payload } = pubRecord;
      this.client.publish(topic, payload, { qos }, (error) => {
        if (error) {
          console.log("Publish error: ", error);
        }
      });
    }
  };

  handleDisconnect = () => {
    if (this.client) {
      this.client.end(() => {
        this.setState({ connectStatus: "Connect" });
        this.setState({ client: null });
      });
    }
  };

  render() {
    return (
      <>
        <Connection
          connectBtn={this.state.connectStatus}
          connect={this.handleConnect}
          disconnect={this.handleDisconnect}
        />
        <QosOption.Provider value={qosOption}>
          <Subscriber
            showUnsub={this.state.isSubed}
            subscribe={this.handleSubscribe}
            unsubscribe={this.handleUnsub}
          />
          <Publisher publish={this.handlePublish} />
        </QosOption.Provider>
        <Receiver messages={this.state.messages}></Receiver>
      </>
    );
  }
}

export default ClassMqtt;
