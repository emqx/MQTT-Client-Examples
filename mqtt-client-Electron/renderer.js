// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// No Node.js APIs are available in this process because
// `nodeIntegration` is turned off. Use `preload.js` to
// selectively enable features needed in the rendering
// process.
let client = null

const options = {
  keepalive: 30,
  protocolId: 'MQTT',
  protocolVersion: 4,
  clean: true,
  reconnectPeriod: 1000,
  connectTimeout: 30 * 1000,
  will: {
    topic: 'WillMsg',
    payload: 'Connection Closed abnormally..!',
    qos: 0,
    retain: false
  },
  rejectUnauthorized: false
}
const connectBtn = document.getElementById('connectBtn')
const disconnectBtn = document.getElementById('disconnectBtn')
const subBtn = document.getElementById('subBtn')
const unsubBtn = document.getElementById('unsubBtn')
const sendBtn = document.getElementById('sendBtn')

connectBtn.addEventListener('click', onConnect)
disconnectBtn.addEventListener('click', onDisconnect)
subBtn.addEventListener('click', onSub)
unsubBtn.addEventListener('click', onUnsub)
sendBtn.addEventListener('click', onSend)

function onConnect () {
  const { host, port, clientId, username, password } = connection
  const connectUrl = `mqtt://${host.value}:${port.value}`
  options.clientId = clientId.value || `mqttjs_${Math.random().toString(16).substr(2, 8)}`
  options.username = username.value
  options.password = password.value
  console.log('connecting mqtt client')
  client = mqtt.connect(connectUrl, options)
  client.on('error', (err) => {
    console.error('Connection error: ', err)
    client.end()
  })
  client.on('reconnect', () => {
    console.log('Reconnecting...')
  })
  client.on('connect', () => {
    console.log('Client connected:' + options.clientId)
    connectBtn.innerText = 'Connected'
  })
  client.on('message', (topic, message) => {
    const msg = document.createElement('div')
    msg.className = 'message-body'
    msg.innerText = `${message.toString()}\nOn topic: ${topic}`
    document.getElementById('article').appendChild(msg)
  })
}

function onDisconnect () {
  if (client.connected) {
    client.end()
    client.on('close', () => {
      connectBtn.innerText = 'Connect'
      console.log(options.clientId + ' disconnected')
    })
  }
}

function onSub () {
  if (client.connected) {
    const { topic, qos } = subscriber
    client.subscribe(topic.value, { qos: parseInt(qos.value, 10) }, (error, res) => {
       if (error) {
         console.error('Subscribe error: ', error)
       } else {
         console.log('Subscribed: ', res)
       }
    })
  }
}

function onUnsub () {
  if (client.connected) {
    const { topic } = subscriber
    client.unsubscribe(topic.value, error => {
      if (error) {
        console.error('Unsubscribe error: ', error)
      } else {
        console.log('Unsubscribed: ', topic.value)
      }
    })
  }
}

function onSend () {
  if (client.connected) {
    const { topic, qos, payload } = publisher
    client.publish(topic.value, payload.value, {
      qos: parseInt(qos.value, 10),
      retain: false
    })
  }
}
