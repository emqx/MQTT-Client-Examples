const fs = require('fs')

const connectOptions = {
  protocol: 'wss',
  port: 8084,
  host: 'broker.emqx.io',
  ca: fs.readFileSync('./broker.emqx.io-ca.crt'),
}

module.exports = {
  connectOptions,
}
