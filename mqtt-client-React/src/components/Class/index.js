import React, { createContext } from 'react'
import Connection from './Connection'
import Publisher from './Publisher'
import Subscriber from './Subscriber'
import Receiver from './Receiver'
import mqtt from 'mqtt'

export const QosOption = createContext([])
// https://github.com/mqttjs/MQTT.js#qos
const qosOption = [
  {
    label: '0',
    value: 0,
  },
  {
    label: '1',
    value: 1,
  },
  {
    label: '2',
    value: 2,
  },
]

class ClassMqtt extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      client: null,
      connectStatus: 'Connect',
      isSubed: false,
      messages: [],
    }
  }

  handleConnect = (host, mqttOptions) => {
    this.setState({ connectStatus: 'Connecting' })
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
    this.client = mqtt.connect(host, mqttOptions)

    if (this.client) {
      // https://github.com/mqttjs/MQTT.js#event-connect
      this.client.on('connect', () => {
        this.setState({ connectStatus: 'Connected' })
        console.log('connection successful')
      })

      // https://github.com/mqttjs/MQTT.js#event-error
      this.client.on('error', (err) => {
        console.error('Connection error: ', err)
        this.client.end()
      })

      // https://github.com/mqttjs/MQTT.js#event-reconnect
      this.client.on('reconnect', () => {
        this.setState({ connectStatus: 'Reconnecting' })
      })

      // https://github.com/mqttjs/MQTT.js#event-message
      this.client.on('message', (topic, message) => {
        const payload = { topic, message: message.toString() }
        const { messages } = this.state
        if (payload.topic) {
          const changedMessages = messages.concat([payload])
          this.setState({ messages: changedMessages })
        }
        console.log(`received message: ${message} from topic: ${topic}`)
      })
    }
  }

  handleSubscribe = (topic, qos) => {
    if (this.client) {
      // subscribe topic
      // https://github.com/mqttjs/MQTT.js#mqttclientsubscribetopictopic-arraytopic-object-options-callback
      this.client.subscribe(topic, { qos }, (error) => {
        if (error) {
          console.log('Subscribe to topics error', error)
          return
        }
        console.log(`Subscribe to topics: ${topic}`)
        this.setState({ isSubed: true })
      })
    }
  }

  // unsubscribe topic
  // https://github.com/mqttjs/MQTT.js#mqttclientunsubscribetopictopic-array-options-callback
  handleUnsub = (topic, qos) => {
    if (this.client) {
      this.client.unsubscribe(topic, { qos }, (error) => {
        if (error) {
          console.log('Unsubscribe error', error)
          return
        }
        console.log(`unsubscribed topic: ${topic}`)
        this.setState({ isSubed: false })
      })
    }
  }

  // publish message
  // https://github.com/mqttjs/MQTT.js#mqttclientpublishtopic-message-options-callback
  handlePublish = (pubRecord) => {
    if (this.client) {
      const { topic, qos, payload } = pubRecord
      this.client.publish(topic, payload, { qos }, (error) => {
        if (error) {
          console.log('Publish error: ', error)
        }
      })
    }
  }

  // disconnect
  // https://github.com/mqttjs/MQTT.js#mqttclientendforce-options-callback
  handleDisconnect = () => {
    if (this.client) {
      try {
        this.client.end(false, () => {
          this.setState({ connectStatus: 'Connect' })
          this.setState({ client: null })
          console.log('disconnected successfully')
        })
      } catch (error) {
        this.setState({ connectStatus: 'Connect' })
        console.log('disconnect error:', error)
      }
    }
  }

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
    )
  }
}

export default ClassMqtt
