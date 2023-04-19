<script setup lang="ts">
// vue 3 + vite use MQTT.js refer to https://github.com/mqttjs/MQTT.js/issues/1269
import * as mqtt from "mqtt/dist/mqtt.min";
import { reactive, ref } from "vue";

// https://github.com/mqttjs/MQTT.js#qos
const qosList = [0, 1, 2];

/**
 * this demo uses EMQX Public MQTT Broker (https://www.emqx.com/en/mqtt/public-mqtt5-broker), here are the details:
 *
 * Broker host: broker.emqx.io
 * WebSocket port: 8083
 * WebSocket over TLS/SSL port: 8084
 */
const connection = reactive({
  // ws or wss
  protocol: "ws",
  host: "broker.emqx.io",
  // ws -> 8083; wss -> 8084
  port: 8083,
  clientId: "emqx_vue3_" + Math.random().toString(16).substring(2, 8),
  /**
   * By default, EMQX allows clients to connect without authentication.
   * https://docs.emqx.com/en/enterprise/v4.4/advanced/auth.html#anonymous-login
   */
  username: "emqx_test",
  password: "emqx_test",
  clean: true,
  connectTimeout: 30 * 1000, // ms
  reconnectPeriod: 4000, // ms
  // for more options and details, please refer to https://github.com/mqttjs/MQTT.js#mqttclientstreambuilder-options
});

// topic & QoS for MQTT subscribing
const subscription = ref({
  topic: "topic/mqttx",
  qos: 0 as mqtt.QoS,
});

// topic, QoS & payload for publishing message
const publish = ref({
  topic: "topic/browser",
  qos: 0 as mqtt.QoS,
  payload: '{ "msg": "Hello, I am browser." }',
});

let client = ref({
  connected: false,
} as mqtt.MqttClient);
const receivedMessages = ref("");
const subscribedSuccess = ref(false);
const btnLoadingType = ref("");
const retryTimes = ref(0);

const initData = () => {
  client.value = {
    connected: false,
  } as mqtt.MqttClient;
  retryTimes.value = 0;
  btnLoadingType.value = "";
  subscribedSuccess.value = false;
};

const handleOnReConnect = () => {
  retryTimes.value += 1;
  if (retryTimes.value > 5) {
    try {
      client.value.end();
      initData();
      console.log("connection maxReconnectTimes limit, stop retry");
    } catch (error) {
      console.log("handleOnReConnect catch error:", error);
    }
  }
};

// create MQTT connection
const createConnection = () => {
  try {
    btnLoadingType.value = "connect";
    const { protocol, host, port, ...options } = connection;
    const connectUrl = `${protocol}://${host}:${port}/mqtt`;

    /**
     * if protocol is "ws", connectUrl = "ws://broker.emqx.io:8083/mqtt"
     * if protocol is "wss", connectUrl = "wss://broker.emqx.io:8084/mqtt"
     * 
     * /mqtt: MQTT-WebSocket uniformly uses /path as the connection path,
     * which should be specified when connecting, and the path used on EMQX is /mqtt.
     * 
     * for more details about "mqtt.connect" method & options,
     * please refer to https://github.com/mqttjs/MQTT.js#mqttconnecturl-options
     */
    client.value = mqtt.connect(connectUrl, options);

    if (client.value.on) {
      // https://github.com/mqttjs/MQTT.js#event-connect
      client.value.on("connect", () => {
        btnLoadingType.value = "";
        console.log("connection successful");
      });

      // https://github.com/mqttjs/MQTT.js#event-reconnect
      client.value.on("reconnect", handleOnReConnect);

      // https://github.com/mqttjs/MQTT.js#event-error
      client.value.on("error", (error) => {
        console.log("connection error:", error);
      });

      // https://github.com/mqttjs/MQTT.js#event-message
      client.value.on("message", (topic: string, message) => {
        receivedMessages.value = receivedMessages.value.concat(
          message.toString()
        );
        console.log(`received message: ${message} from topic: ${topic}`);
      });
    }
  } catch (error) {
    btnLoadingType.value = "";
    console.log("mqtt.connect error:", error);
  }
};

// subscribe topic
// https://github.com/mqttjs/MQTT.js#mqttclientsubscribetopictopic-arraytopic-object-options-callback
const doSubscribe = () => {
  btnLoadingType.value = "subscribe";
  const { topic, qos } = subscription.value;
  client.value.subscribe(
    topic,
    { qos },
    (error: Error, granted: mqtt.ISubscriptionGrant[]) => {
      btnLoadingType.value = "";
      if (error) {
        console.log("subscribe error:", error);
        return;
      }
      subscribedSuccess.value = true;
      console.log("subscribe successfully:", granted);
    }
  );
};

// unsubscribe topic
// https://github.com/mqttjs/MQTT.js#mqttclientunsubscribetopictopic-array-options-callback
const doUnSubscribe = () => {
  btnLoadingType.value = "unsubscribe";
  const { topic, qos } = subscription.value;
  client.value.unsubscribe(topic, { qos }, (error) => {
    btnLoadingType.value = "";
    subscribedSuccess.value = false;
    if (error) {
      console.log("unsubscribe error:", error);
      return;
    }
    console.log(`unsubscribed topic: ${topic}`);
  });
};

// publish message
// https://github.com/mqttjs/MQTT.js#mqttclientpublishtopic-message-options-callback
const doPublish = () => {
  btnLoadingType.value = "publish";
  const { topic, qos, payload } = publish.value;
  client.value.publish(topic, payload, { qos }, (error) => {
    btnLoadingType.value = "";
    if (error) {
      console.log("publish error:", error);
      return;
    }
    console.log(`published message: ${payload}`);
  });
};

// disconnect
// https://github.com/mqttjs/MQTT.js#mqttclientendforce-options-callback
const destroyConnection = () => {
  if (client.value.connected) {
    btnLoadingType.value = "disconnect";
    try {
      client.value.end(false, () => {
        initData();
        console.log("disconnected successfully");
      });
    } catch (error) {
      btnLoadingType.value = "";
      console.log("disconnect error:", error);
    }
  }
};

const handleProtocolChange = (value: string) => {
  connection.port = value === "wss" ? 8084 : 8083;
};
</script>

<template>
  <div class="mqtt-demo">
    <el-card>
      <h1>Configuration</h1>
      <el-form label-position="top" :model="connection">
        <el-row :gutter="20">
          <el-col :span="8">
            <el-form-item prop="protocol" label="Protocol">
              <el-select
                v-model="connection.protocol"
                @change="handleProtocolChange"
              >
                <el-option label="ws://" value="ws"></el-option>
                <el-option label="wss://" value="wss"></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item prop="host" label="Host">
              <el-input v-model="connection.host"></el-input>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item prop="port" label="Port">
              <el-input
                v-model.number="connection.port"
                type="number"
                placeholder="8083/8084"
              ></el-input>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item prop="clientId" label="Client ID">
              <el-input v-model="connection.clientId"> </el-input>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item prop="username" label="Username">
              <el-input v-model="connection.username"></el-input>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item prop="password" label="Password">
              <el-input v-model="connection.password"></el-input>
            </el-form-item>
          </el-col>

          <el-col :span="24">
            <el-button
              type="primary"
              :disabled="client.connected"
              @click="createConnection"
              :loading="btnLoadingType === 'connect'"
            >
              {{ client.connected ? "Connected" : "Connect" }}
            </el-button>

            <el-button
              v-if="client.connected"
              type="danger"
              @click="destroyConnection"
              :loading="btnLoadingType === 'disconnect'"
            >
              Disconnect
            </el-button>
          </el-col>
        </el-row>
      </el-form>
    </el-card>
    <el-card>
      <h1>Subscribe</h1>
      <el-form label-position="top" :model="subscription">
        <el-row :gutter="20">
          <el-col :span="8">
            <el-form-item prop="topic" label="Topic">
              <el-input
                v-model="subscription.topic"
                :disabled="subscribedSuccess"
              ></el-input>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item prop="qos" label="QoS">
              <el-select
                v-model="subscription.qos"
                :disabled="subscribedSuccess"
              >
                <el-option
                  v-for="qos in qosList"
                  :key="qos"
                  :label="qos"
                  :value="qos"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-button
              type="primary"
              class="sub-btn"
              :loading="btnLoadingType === 'subscribe'"
              :disabled="!client.connected || subscribedSuccess"
              @click="doSubscribe"
            >
              {{ subscribedSuccess ? "Subscribed" : "Subscribe" }}
            </el-button>
            <el-button
              v-if="subscribedSuccess"
              type="primary"
              class="sub-btn"
              :loading="btnLoadingType === 'unsubscribe'"
              :disabled="!client.connected"
              @click="doUnSubscribe"
            >
              Unsubscribe
            </el-button>
          </el-col>
        </el-row>
      </el-form>
    </el-card>
    <el-card>
      <h1>Publish</h1>
      <el-form label-position="top" :model="publish">
        <el-row :gutter="20">
          <el-col :span="8">
            <el-form-item prop="topic" label="Topic">
              <el-input v-model="publish.topic"></el-input>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item prop="payload" label="Payload">
              <el-input v-model="publish.payload"></el-input>
            </el-form-item>
          </el-col>
          <el-col :span="8">
            <el-form-item prop="qos" label="QoS">
              <el-select v-model="publish.qos">
                <el-option
                  v-for="qos in qosList"
                  :key="qos"
                  :label="qos"
                  :value="qos"
                ></el-option>
              </el-select>
            </el-form-item>
          </el-col>
        </el-row>
      </el-form>
      <el-col :span="24" class="text-right">
        <el-button
          type="primary"
          :loading="btnLoadingType === 'publish'"
          :disabled="!client.connected"
          @click="doPublish"
        >
          Publish
        </el-button>
      </el-col>
    </el-card>
    <el-card>
      <h1>Receive</h1>
      <el-col :span="24">
        <el-input
          type="textarea"
          :rows="3"
          v-model="receivedMessages"
          readonly
        ></el-input>
      </el-col>
    </el-card>
  </div>
</template>

<style>
.mqtt-demo {
  max-width: 1200px;
  margin: 32px auto 0 auto;
}

h1 {
  font-size: 16px;
  margin-top: 0;
}

.el-card {
  margin-bottom: 32px;
}
.el-card__body {
  padding: 24px;
}

.el-select {
  width: 100%;
}

.text-right {
  text-align: right;
}

.sub-btn {
  margin-top: 30px;
}
</style>
