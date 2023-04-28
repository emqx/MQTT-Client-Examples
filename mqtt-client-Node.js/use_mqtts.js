const fs = require('fs')

const connectOptions = {
  protocol: 'mqtts',
  port: 8883,
  host: 'broker.emqx.io',
  ca: fs.readFileSync('./broker.emqx.io-ca.crt'),
}

module.exports = {
  connectOptions,
}
