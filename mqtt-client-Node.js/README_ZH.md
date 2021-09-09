# Node.js MQTT 客户端使用示例

在本示例中我们提供了 TCP、Websocket、TLS、Websockets 协议连接到 MQTT Broker 示例代码。
有关 Node.js MQTT.js 客户端的更多使用文档, 请参阅 [MQTT.js 文档](https://github.com/mqttjs/MQTT.js)

## 准备工作

* 安装 Node.js 和 NPM

* 安装依赖

  ```bash
  npm install
  // 或者使用 yarn
  yarn
  ```

* 运行

  ```bash
  npm start
  // 或者使用 yarn
  yarn start
  ```

## 使用说明

```bash
node index.js -p mqtt

Options:
  -p, --protocol <type>  connect protocol: mqtt, mqtts, ws, wss. default is mqtt (default: "mqtt")
  -h, --help             display help for command
```
