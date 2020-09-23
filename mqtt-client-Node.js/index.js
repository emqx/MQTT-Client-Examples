const fs = require('fs')
const mqtt = require('mqtt')
const { Command } = require('commander')

const program = new Command()
program
  .option('-p, --protocol <type>', 'connect protocol: mqtt, mqtts, ws, wss. default is mqtt', 'mqtt')
  .parse(process.argv)


const HOST = 'broker.emqx.io'
const DEF_PORT = 1883
const TOPIC= `node-js/${program.protocol}`

// connect options
const OPTIONS = {
  clean: true,
  connectTimeout: 4000,
  clientId: `mqtt_${Math.random().toString(16).slice(3)}`,
  username: 'emqx',
  password: 'public',
  reconnectPeriod: 1000,
}
// protocol list
const PROTOCOLS = ['mqtt', 'mqtts', 'ws', 'wss']

// default is mqtt, unencrypted tcp connection
let connectUrl = `mqtt://${HOST}:${DEF_PORT}`
if (program.protocol && PROTOCOLS.indexOf(program.protocol) === -1) {
  console.log('protocol must one of mqtt, mqtts, ws, wss.')
} else if (program.protocol === 'mqtts') {
  // mqtts， encrypted tcp connection
  connectUrl = `mqtts://${HOST}:8883`
  OPTIONS['ca'] = fs.readFileSync('./broker.emqx.io-ca.crt')
} else if (program.protocol === 'ws') {
  // ws, unencrypted WebSocket connection
  const mountPath = '/mqtt' // mount path, connect emqx via WebSocket
  connectUrl = `ws://${HOST}:8083${mountPath}`
} else if (program.protocol === 'wss') {
  // wss, encrypted WebSocket connection
  const mountPath = '/mqtt' // mount path, connect emqx via WebSocket
  connectUrl = `wss://${HOST}:8084${mountPath}`
  OPTIONS['ca'] = fs.readFileSync('./broker.emqx.io-ca.crt')
} else {}

const client = mqtt.connect(connectUrl, OPTIONS)

client.on('connect', () => {
  console.log(`${program.protocol}: Connected`)
  client.subscribe([TOPIC], () => {
    console.log(`${program.protocol}: Subscribe to topic '${TOPIC}'`)
  })
})

client.on('reconnect', (error) => {
  console.log(`Reconnecting(${program.protocol}):`, error)
})

client.on('error', (error) => {
  console.log(`Cannot connect(${program.protocol}):`, error)
})

client.on('message', (topic, payload) => {
  console.log('Received Message:', topic, payload.toString())
})
