// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// No Node.js APIs are available in this process because
// `nodeIntegration` is turned off. Use `preload.js` to
// selectively enable features needed in the rendering
// process.
let client = null

/**
 * this demo uses EMQX Public MQTT Broker (https://www.emqx.com/en/mqtt/public-mqtt5-broker), here are the details:
 *
 * Broker host: broker.emqx.io
 * TCP Port: 1883
 * SSL/TLS Port: 8883
 * WebSocket port: 8083
 * WebSocket over TLS/SSL port: 8084
 */

const options = {
  keepalive: 30,
  protocolId: 'MQTT',
  protocolVersion: 4,
  clean: true,
  connectTimeout: 30 * 1000, // ms
  reconnectPeriod: 4000, // ms
  // for more options, please refer to https://github.com/mqttjs/MQTT.js#mqttclientstreambuilder-options
}

const protocolSelect = document.getElementById('protocol-select')
const connectBtn = document.getElementById('connect-btn')
const disconnectBtn = document.getElementById('disconnect-btn')
const subBtn = document.getElementById('sub-btn')
const unsubBtn = document.getElementById('unsub-btn')
const sendBtn = document.getElementById('send-btn')
const pathIpt = document.getElementById('path-ipt')
const fileArea = document.getElementById('file-area')
const caIpt = document.getElementById('ca')
const clientCertIpt = document.getElementById('client-cert')
const clientKeyIpt = document.getElementById('clientKey')
const caFileName = document.getElementById('caFileName')
const clientCertFileName = document.getElementById('client-cert-file-name')
const clientFileName = document.getElementById('client-key-file-name')

protocolSelect.addEventListener('change', onProtocolChange)
connectBtn.addEventListener('click', onConnect)
disconnectBtn.addEventListener('click', onDisconnect)
subBtn.addEventListener('click', onSub)
unsubBtn.addEventListener('click', onUnsub)
sendBtn.addEventListener('click', onSend)
caIpt.addEventListener('change', handleFileUpdate(caFileName, 'ca'))
clientCertIpt.addEventListener(
  'change',
  handleFileUpdate(clientCertFileName, 'cert')
)
clientKeyIpt.addEventListener('change', handleFileUpdate(clientFileName, 'key'))

// type => ca、cert、key     Representing server-ca, client-certificate, and client-key respectively
function handleFileUpdate(fileName, type) {
  return function (e) {
    const file = e.target.files[0]
    fileName.innerText = file.name
    if (file) {
      const reader = new FileReader()
      reader.onload = (e) => {
        const fileContent = e.target.result
        // Transfer TLS/SSL related files
        options[type] = fileContent
      }
      reader.readAsText(file)
    }
  }
}

//  protocol->port    mqtt: 1883; mqtts: 8883; ws: 8083; wss: 8084
function onProtocolChange() {
  switch (connection.protocol.value) {
    case 'mqtt':
      connection.port.value = 1883
      pathIpt.style.display = 'none'
      fileArea.style.display = 'none'
      break
    case 'mqtts':
      connection.port.value = 8883
      pathIpt.style.display = 'none'
      fileArea.style.display = 'block'
      break
    case 'ws':
      connection.port.value = 8083
      pathIpt.style.display = 'block'
      fileArea.style.display = 'none'
      break
    case 'wss':
      connection.port.value = 8084
      pathIpt.style.display = 'block'
      fileArea.style.display = 'block'
      break
    default:
      break
  }
}

// create MQTT connection
function onConnect() {
  const { protocol, host, port, clientId, username, password, path } =
    connection

  let connectUrl = `${protocol.value}://${host.value}:${port.value}`
  if (protocol.value === 'ws' || protocol.value === 'wss') {
    connectUrl = connectUrl + path.value
  }

  options.clientId =
    clientId.value ||
    `emqx_electron_${Math.random().toString(16).substring(2, 10)}`

  /**
   * By default, EMQX allows clients to connect without authentication.
   * https://docs.emqx.com/en/enterprise/v4.4/advanced/auth.html#anonymous-login
   */
  options.username = username.value
  options.password = password.value
  console.log('connecting mqtt client')

  /**
   * if protocol is "mqtt", connectUrl = "mqtt://broker.emqx.io:1883"
   * if protocol is "mqtts", connectUrl = "mqtts://broker.emqx.io:8883"
   * if protocol is "ws", connectUrl = "ws://broker.emqx.io:8083/mqtt"
   * if protocol is "wss", connectUrl = "wss://broker.emqx.io:8084/mqtt"
   *
   * /mqtt: MQTT-WebSocket uniformly uses /path as the connection path,
   * which should be specified when connecting, and the path used on EMQX is /mqtt.
   *
   * Note: Here we use the mqtts or wss protocol with TLS one-way authentication,
   * which only verifies the server's certificate and does not verify the client's certificate.
   * If you require two-way authentication, please use TLS two-way authentication.
   *
   * for more details about "mqtt.connect" method & options,
   * please refer to https://github.com/mqttjs/MQTT.js#mqttconnecturl-options
   */
  client = mqtt.connect(connectUrl, options)

  // https://github.com/mqttjs/MQTT.js#event-connect
  client.on('connect', () => {
    console.log('Client connected:' + options.clientId)
    connectBtn.innerText = 'Connected'
  })

  // https://github.com/mqttjs/MQTT.js#event-reconnect
  client.on('reconnect', () => {
    console.log('Reconnecting...')
  })

  // https://github.com/mqttjs/MQTT.js#event-error
  client.on('error', (err) => {
    console.error('Connection error: ', err)
    client.end()
  })

  // https://github.com/mqttjs/MQTT.js#event-message
  client.on('message', (topic, message) => {
    const msg = document.createElement('div')
    msg.className = 'message-body'
    msg.innerText = `${message.toString()}\nOn topic: ${topic}`
    document.getElementById('article').appendChild(msg)
  })
}

// disconnect
// https://github.com/mqttjs/MQTT.js#mqttclientendforce-options-callback
function onDisconnect() {
  if (client.connected) {
    // https://github.com/mqttjs/MQTT.js#end
    client.end()
    client.on('close', () => {
      connectBtn.innerText = 'Connect'
      console.log(options.clientId + ' disconnected')
    })
  }
}

// subscribe topic
// https://github.com/mqttjs/MQTT.js#mqttclientsubscribetopictopic-arraytopic-object-options-callback
function onSub() {
  if (client.connected) {
    // https://github.com/mqttjs/MQTT.js#qos
    const { topic, qos } = subscriber
    client.subscribe(
      topic.value,
      { qos: parseInt(qos.value, 10) },
      (error, res) => {
        if (error) {
          console.error('Subscribe error: ', error)
        } else {
          console.log('Subscribed: ', res)
        }
      }
    )
  }
}

// unsubscribe topic
// https://github.com/mqttjs/MQTT.js#mqttclientunsubscribetopictopic-array-options-callback
function onUnsub() {
  if (client.connected) {
    const { topic } = subscriber
    client.unsubscribe(topic.value, (error) => {
      if (error) {
        console.error('Unsubscribe error: ', error)
      } else {
        console.log('Unsubscribed: ', topic.value)
      }
    })
  }
}

// publish message
// https://github.com/mqttjs/MQTT.js#mqttclientpublishtopic-message-options-callback
function onSend() {
  if (client.connected) {
    const { topic, qos, payload } = publisher
    client.publish(topic.value, payload.value, {
      qos: parseInt(qos.value, 10),
      retain: false,
    })
  }
}
