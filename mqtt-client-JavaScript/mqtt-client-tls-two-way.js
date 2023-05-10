const mqtt = require('mqtt')
const fs = require('fs')
const path = require('path')

/***
 * Node.js
 * Using MQTT over TCP with mqtt and mqtts protocols
 * EMQX's mqtt connection default port is 1883, mqtts is 8883
 * Two-way TLS only works in Node.js using MQTT over TCP, not in the browser. Ref: https://github.com/mqttjs/MQTT.js/issues/1515  
 */
const url = 'mqtts://broker.emqx.io:8883'
const KEY = fs.readFileSync(path.join(__dirname, '/client.key'))
const CERT = fs.readFileSync(path.join(__dirname, '/client.pem'))
const CA_CERT = fs.readFileSync(path.join(__dirname, './server-ca.crt'))

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
  key: KEY,
  cert: CERT,
  ca: CA_CERT,
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