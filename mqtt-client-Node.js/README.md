# Node.js MQTT Client Examples

In this example we provide example code for TCP, Websocket, TLS, and Websockets protocols to connect to the MQTT Broker. There are separate files for using TLS and two-way TLS authentication: `index-tls.js` and `index-tls-two-way.js`.

For more documentation on the use of the Javascript MQTT.js client, see the [Javascript Client - documentation](https://github.com/mqttjs/MQTT.js)

## Installation

- Install Node.js and NPM

- Installation dependencies

  ```bash
  npm install
  // or use yarn
  yarn
  ```

- Start

  ```bash
  npm start
  // or use yarn
  yarn start
  ```

## Usage

For basic usage (TCP or Websocket), use the index.js file:

```bash
node index.js
```

For a TLS connection, use the index-tls.js file:

```bash
node index-tls.js
```

For a two-way TLS connection, use the index-tls-two-way.js file:

```bash
node index-tls-two-way.js
```
