// All of the Node.js APIs are available in the preload process.
// It has the same sandbox as a Chrome extension.
const mqtt = require('mqtt')
window.mqtt = mqtt
