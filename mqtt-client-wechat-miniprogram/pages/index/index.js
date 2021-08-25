import mqtt from '../../utils/mqtt'

Page({
  data: {
    btnText: '连接',
    client: null,
    host: 'broker.emqx.io:8084',
    topic: 'testtopic/miniprogram',
    msg: 'Hello! I am from WeChat miniprogram',
    mqttOptions: {
      protocolVersion: 4, //MQTT连接协议版本
      clientId: 'emqx_cloud_miniprogram',
      clean: true,
      password: '',
      username: '',
      reconnectPeriod: 1000, // 1000毫秒，两次重新连接之间的间隔
      connectTimeout: 30 * 1000, // 1000毫秒，两次重新连接之间的间隔
      resubscribe: true // 如果连接断开并重新连接，则会再次自动订阅已订阅的主题（默认true）
    },
  },
  setHost(e) {
    this.setData({
      host: e.detail.value,
    })
  },
  setTopic(e) {
    this.setData({
      topic: e.detail.value,
    })
  },
  setMsg(e) {
    this.setData({
      msg: e.detail.value,
    })
  },
  connect() {
    this.data.client = mqtt.connect(`wxs://${this.data.host}/mqtt`, this.data.mqttOptions)
    this.data.client.on('connect', () => {
      wx.showToast({
        title: '连接成功'
      })
      this.setData({
        btnText: '已连接',
      })
      this.data.client.on('message', (topic, payload) => {
        wx.showModal({
          content: `收到 Topic: ${topic}, Payload: ${payload}`,
          showCancel: false,
        });
      })
      this.data.client.on('error', (error) => {
        console.log('error')
      })
      this.data.client.on('reconnect', () => {
        console.log('Reconnecting...')
      })
      this.data.client.on('offline', (errr) => {
        console.log('Offline')
      })
    })
  },
  disconnect() {
    this.data.client.end()
    this.setData({
      btnText: '连接',
    })
    wx.showToast({
      title: '断开连接成功'
    })
  },
  subscribe() {
    this.data.client.subscribe(this.data.topic)
    wx.showToast({
      title: `主题订阅成功`
    })
  },
  unsubscribe() {
    this.data.client.unsubscribe(this.data.topic)
    wx.showToast({
      title: `取消订阅成功`
    })
  },
  publish() {
    this.data.client.publish(this.data.topic, this.data.msg)
  },
})
