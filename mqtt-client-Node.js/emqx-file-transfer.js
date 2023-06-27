/**
 * File Transfer over MQTT
 * @emqx EMQX Enterprise v5.1.0+
 * @document https://docs.emqx.com/en/enterprise/v5.1/file-transfer/introduction.html
 */

const fs = require('fs')
const path = require('path')
const mqtt = require('mqtt')

const FILE_PATH = path.join(__dirname, '../assets/emqx-architecture.png')
// File segment size, default is 10KB, muse be less than `mqtt.max_packet_size` (default is 1024KB)
const SEGMENT_SIZE = 1024 * 10

// Only support EMQX Enterprise v5.1.0+ and need to enable file transfer
const HOST = 'mqtt://localhost:1883'
const CLIENT_ID = 'emqx-file-transfer-' + Math.random().toString(16).substring(2, 4)

// Get file information
const fileSize = fs.statSync(FILE_PATH).size
const fileName = FILE_PATH.split('/').pop()
const fileChecksum = calculateChecksum(FILE_PATH)

const client = mqtt.connect(HOST, {
  clientId: CLIENT_ID
})

client.on('connect', async () => {
  // Initialize file transfer session
  const initCommand = {
    name: fileName,
    size: fileSize,
    checksum: fileChecksum,
  }

  const timeStart = Date.now()

  // Use file checksum as file_id
  const initTopic = `$file/${fileChecksum}/init`
  const packet = await publishMessage(initTopic, JSON.stringify(initCommand), { qos: 1 })
  console.log('File transfer session initialized.', packet)

  // Read file and publish segments
  const fileStream = fs.createReadStream(FILE_PATH, { highWaterMark: SEGMENT_SIZE })
  let offset = 0

  fileStream.on('data', async (chunk) => {
    // Publish file segment, the offset is file segment start position
    const segmentTopic = `$file/${fileChecksum}/${offset}`
    offset += chunk.length
    let _offset = offset
    await publishMessage(segmentTopic, chunk, { qos: 1 })
    console.log(`File segment ${_offset} sent.`)
  })

  fileStream.on('end', async () => {
    const finishTopic = `$file/${fileChecksum}/fin/${fileSize}`
    const packet = await publishMessage(finishTopic, null, { qos: 1 })
    console.log('File transfer finished.', packet)
    const timeEnd = Date.now()
    console.table({
      'File ID': fileChecksum,
      'File Size': bytesFormatter(fileSize),
      'Transfer Time': (timeEnd - timeStart) / 1000 + ' seconds',
      'Transfer Speed': bytesFormatter((fileSize / (timeEnd - timeStart) * 1000)) + '/s'
    })
    client.end()
  })
})

/**
 * Publish a message to the MQTT broker
 * @param {string} topic MQTT topic to publish the message to
 * @param {string | Buffer} message The message payload
 * @param {Object} options Additional options for publishing (e.g., qos)
 * @returns {Promise} A promise that resolves when the message is published
 */
function publishMessage(topic, message, options) {
  return new Promise((resolve, reject) => {
    client.publish(topic, message, options, (error, packet) => {
      if (!error) {
        resolve(packet)
      } else {
        reject(error)
      }
    })
  })
}

/**
 * Calculate file checksum
 * @param {string} filePath file path
 * @return {*}
 */
function calculateChecksum(filePath) {
  const crypto = require('crypto')
  const hash = crypto.createHash('sha256')
  const fileData = fs.readFileSync(filePath)
  hash.update(fileData)
  return hash.digest('hex')
}


/**
 * Format file size
 * @param {number} fileSize
 * @return {*} 
 */
function bytesFormatter(fileSize) {
  const units = ['bytes', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB']
  let index = 0
  let size = fileSize
  while (size > 1024) {
    size /= 1024
    index++
  }
  return size.toFixed(2) + ' ' + units[index]
}