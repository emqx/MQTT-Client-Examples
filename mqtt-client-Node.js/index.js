const mqtt = require('mqtt')
const fs = require('fs')
/* 
  choose which protocol to use for connection here 
*/
// const { connectOptions } = require('./use_mqtt.js')
// const { connectOptions } = require('./use_mqtts.js')
// const { connectOptions } = require('./use_ws.js')
const { connectOptions } = require('./use_wss.js')

/**
 * this demo uses EMQX Public MQTT Broker (https://www.emqx.com/en/mqtt/public-mqtt5-broker), here are the details:
 *
 * Broker host: broker.emqx.io
 * TCP Port: 1883
 * SSL/TLS Port: 8883
 * WebSocket port: 8083
 * WebSocket over TLS/SSL port: 8084
 */

const clientId = 'emqx_nodejs_' + Math.random().toString(16).substring(2, 8)
const options = {
  clientId,
  clean: true,
  connectTimeout: 4000,
  /**
   * By default, EMQX allows clients to connect without authentication.
   * https://docs.emqx.com/en/enterprise/v4.4/advanced/auth.html#anonymous-login
   */
  username: 'emqx_test',
  password: 'emqx_test',
  reconnectPeriod: 1000,
  // for more options and details, please refer to https://github.com/mqttjs/MQTT.js#mqttclientstreambuilder-options
}

const { protocol, host, port } = connectOptions
/**
 * if protocol is "ws", connectUrl = "ws://broker.emqx.io:8083/mqtt"
 * if protocol is "wss", connectUrl = "wss://broker.emqx.io:8084/mqtt"
 *
 * for more details about "mqtt.connect" method & options,
 * please refer to https://github.com/mqttjs/MQTT.js#mqttconnecturl-options
 */
let connectUrl = `${protocol}://${host}:${port}`
if (['ws', 'wss'].includes(protocol)) {
  // mqtt: MQTT-WebSocket uniformly uses /path as the connection path,
  // which should be specified when connecting, and the path used on EMQX is /mqtt.
  connectUrl += '/mqtt'
}
if (['mqtts', 'wss'].includes(protocol)) {
  options['ca'] = fs.readFileSync('./broker.emqx.io-ca.crt')
}

const client = mqtt.connect(connectUrl, options)

const topic = '/nodejs/mqtt'
const payload = 'nodejs mqtt test'
// https://github.com/mqttjs/MQTT.js#qos
const qos = 0

// https://github.com/mqttjs/MQTT.js#event-connect
client.on('connect', () => {
  console.log(`${protocol}: Connected`)

  // subscribe topic
  // https://github.com/mqttjs/MQTT.js#mqttclientsubscribetopictopic-arraytopic-object-options-callback
  client.subscribe(topic, { qos }, () => {
    console.log(`${protocol}: Subscribe to topic '${topic}'`)
  })

  // publish message
  // https://github.com/mqttjs/MQTT.js#mqttclientpublishtopic-message-options-callback
  client.publish(topic, payload, { qos }, (error) => {
    if (error) {
      console.error(error)
    }
  })
})

// https://github.com/mqttjs/MQTT.js#event-reconnect
client.on('reconnect', (error) => {
  console.log(`Reconnecting(${protocol}):`, error)
})

// https://github.com/mqttjs/MQTT.js#event-error
client.on('error', (error) => {
  console.log(`Cannot connect(${protocol}):`, error)
})

// https://github.com/mqttjs/MQTT.js#event-message
client.on('message', (topic, payload) => {
  console.log('Received Message:', topic, payload.toString())
})

// unsubscribe after 10 seconds
setTimeout(() => {
  // unsubscribe topic
  // https://github.com/mqttjs/MQTT.js#mqttclientunsubscribetopictopic-array-options-callback
  client.unsubscribe(topic, { qos }, (error) => {
    if (error) {
      console.log('unsubscribe error:', error)
      return
    }
    console.log(`unsubscribed topic: ${topic}`)
  })
}, 10000)

// disconnect after 12 seconds
setTimeout(() => {
  if (client.connected) {
    try {
      // disconnect
      // https://github.com/mqttjs/MQTT.js#mqttclientendforce-options-callback
      client.end(false, () => {
        console.log('disconnected successfully')
      })
    } catch (error) {
      btnLoadingType.value = ''
      console.log('disconnect error:', error)
    }
  }
}, 12000)
