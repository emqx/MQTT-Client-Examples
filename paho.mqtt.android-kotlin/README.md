[![](https://jitpack.io/v/hannesa2/paho.mqtt.android.svg)](https://jitpack.io/#hannesa2/paho.mqtt.android)
# MQTT Android Service


The MQTT Android Service is an MQTT client library written in Kotlin.
It has been created to provide reliable open-source implementations of open and standard messaging protocols aimed at new, existing, and emerging
applications for Machine-to-Machine (M2M) and Internet of Things (IoT).
MQTT reflects the inherent physical and cost constraints of device connectivity. Its objectives include effective levels of decoupling between devices and applications, designed to keep markets open and encourage the rapid growth of scalable Web and Enterprise middleware and applications.


## Features
|                     |                    |   |                      |                    |
|---------------------|--------------------|---|----------------------|--------------------|
| MQTT 3.1            | :heavy_check_mark: |   | Automatic Reconnect  | :heavy_check_mark: |
| MQTT 3.1.1          | :heavy_check_mark: |   | Offline Buffering    | :heavy_check_mark: |
| LWT                 | :heavy_check_mark: |   | WebSocket Support    | :heavy_check_mark: |
| SSL / TLS           | :heavy_check_mark: |   | Standard TCP Support | :heavy_check_mark: |
| Message Persistence | :heavy_check_mark: |   |

## Using this MQTT Android Client

#### Jitpack.io 

More details are here https://jitpack.io/#hannesa2/paho.mqtt.android

```
  allprojects {
    repositories {
      ...
      maven { url 'https://jitpack.io' }
    }
  }
```

```
dependencies {
  implementation 'androidx.legacy:legacy-support-v4:1.0.0'
  implementation 'com.github.hannesa2:paho.mqtt.android:$latestVersion'
}
```

#### Android O foreground service

Android >= O you should use it as foreground service

```
val client = MqttAndroidClient(context, uri, clientId).apply {
    setForegroundService(foregroundNotification, 3)
}
```

## License

```
Copyright 2021

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
