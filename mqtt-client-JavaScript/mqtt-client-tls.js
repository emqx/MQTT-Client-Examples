const mqtt = require('mqtt')

/***
 * Browser
 * Using MQTT over WebSocket with ws and wss protocols
 * EMQX's ws connection default port is 8083, wss is 8084
 * Note that you need to add a path after the connection address, such as /mqtt
 */
// const url = 'wss://broker.emqx.io:8084/mqtt'
/***
 * Node.js
 * Using MQTT over TCP with mqtt and mqtts protocols
 * EMQX's mqtt connection default port is 1883, mqtts is 8883
 */
const url = 'mqtts://broker.emqx.io:8883'

// Create an MQTT client instance
const options = {
  // Clean session
  clean: true,
  connectTimeout: 4000,
  // Authentication
  clientId: 'emqx_test',
  username: 'emqx_test',
  password: 'emqx_test',
  rejectUnauthorized: true,
}
const client  = mqtt.connect(url, options)
client.on('connect', function () {
  console.log('Connected')
  // Subscribe to a topic
  client.subscribe('test', function (err) {
    if (!err) {
      // Publish a message to a topic
      client.publish('test', 'Hello mqtt')
    }
  })
})

// Receive messages
client.on('message', function (topic, message) {
  // message is Buffer
  console.log(message.toString())
  client.end()
})