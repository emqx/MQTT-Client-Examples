# Android 连接 MQTT 示例

* 语言：Java
* 客户端库：[Paho](https://github.com/eclipse/paho.mqtt.android)

## 运行

在 Android Studio 中打开项目，连接虚拟机或设备，运行

## TLS

参考 [SSLUtils.java](./app/src/main/java/io/emqx/mqtt/SSLUtils.java)

### 单向 TLS

1. 设置 broker 使用 TLS

   ```Java
   String serverURI = "ssl://"+host+":"+port;
   ```

2. 将 CA 证书放在 assets 文件夹中
3. 设置 `MqttConnectOptions` 使用 TLS

   ```Java
   MqttConnectOptions options = new MqttConnectOptions();
   AssetManager am = context.getAssets();
   options.setSocketFactory(SSLUtils.getSingleSocketFactory(am.open("ca.crt")));
   ```

4. 连接到 broker

   ```Java
   client.connect(options);
   ```

### 双向 TLS

1. 设置 broker 使用 TLS

   ```Java
   String serverURI = "ssl://"+host+":"+port;
   ```

2. 将 CA 证书、客户端证书和客户端密钥放在 assets 文件夹中
3. 设置 MqttConnectOptions 使用 TLS

   ```Java
   MqttConnectOptions options = new MqttConnectOptions();
   AssetManager am = context.getAssets();
   options.setSocketFactory(SSLUtils.getSocketFactory(am.open("ca.crt"),am.open("client.crt"),am.open("client.key"),""));
   ```

4. 连接到 broker

   ```Java
   client.connect(options);
   ```
