# Android MQTT Client Examples

## Prerequisites

* programing: Java
* MQTT Client: [Paho](https://github.com/eclipse/paho.mqtt.android)

## Run

Open the project in Android Studio, connect the virtual machine or device, and run

## TLS

Refer to [SSLUtils.java](./app/src/main/java/io/emqx/mqtt/SSLUtils.java)

### One-way

1. Setup the broker to use TLS

   ```Java
   String serverURI = "ssl://" + host + ":" + port;
   ```

2. Put your CA certificate in the assets folder
3. Setup the `MqttConnectOptions` to use TLS

   ```Java
   MqttConnectOptions options = new MqttConnectOptions();
   AssetManager am = context.getAssets();
   options.setSocketFactory(SSLUtils.getSingleSocketFactory(am.open("ca.crt")));
   ```

4. Connect to the broker

   ```Java
   client.connect(options);
   ```

### Two-way

1. Setup the broker to use TLS

   ```Java
   String serverURI = "ssl://" + host + ":" + port;
   ```

2. Put your CA certificate, client certificate and client key in the assets folder
3. Setup the MqttConnectOptions to use TLS

   ```Java
   MqttConnectOptions options = new MqttConnectOptions();
   AssetManager am = context.getAssets();
   options.setSocketFactory(SSLUtils.getSocketFactory(am.open("ca.crt"),am.open("client.crt"),am.open("client.key"),""));
   ```

4. Connect to the broker

   ```Java
    client.connect(options);
    ```
