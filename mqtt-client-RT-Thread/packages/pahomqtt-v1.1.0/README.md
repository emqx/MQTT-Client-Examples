# paho-mqtt
## 1、介绍
- [Paho MQTT](http://www.eclipse.org/paho/downloads.php) 是 Eclipse 实现的基于 MQTT 协议的客户端，本软件包是在 Eclipse [paho-mqtt](https://github.com/eclipse/paho.mqtt.embedded-c) 源码包的基础上设计的一套 MQTT 客户端程序。

- `paho-mqtt` 软件包功能特点以及 MQTT  协议介绍请参考 [软件包详细介绍](docs/introduction.md)。

### 1.1 目录结构

`paho-mqtt` 软件包目录结构如下所示：

``` 
pahomqtt
├───docs 
│   └───figures                     // 文档使用图片
│   │   api.md                      // API 使用说明
│   │   introduction.md             // 介绍文档
│   │   principle.md                // 实现原理
│   │   README.md                   // 文档结构说明  
│   │   samples.md                  // 软件包示例
│   │   user-guide.md               // 使用说明
│   └───version.md                  // 版本
├───MQTTClient-RT                   // 移植文件
├───MQTTPacket                      // 源文件
├───samples                         // 示例代码
│       mqtt_sample.c               // 软件包应用示例代码
├───tests                           // mqtt 功能测试程序
│   LICENSE                         // 软件包许可证
│   README.md                       // 软件包使用说明
└───SConscript                      // RT-Thread 默认的构建脚本
```

### 1.2 许可证

`paho-mqtt` package 遵循 Eclipse Public License - v 1.0 许可，详见 `LICENSE` 文件。

### 1.3 依赖

- RT-Thread 3.0+

## 2、获取软件包

使用 `paho-mqtt` 软件包需要在 BSP 目录下使用 menuconfig 命令打开 Env 配置界面，在 `RT-Thread online packages → IoT - internet of things`  中选择 Paho MQTT 软件包，操作界面如下图所示：

![选中 Paho MQTT 软件包](docs/figures/select_mqtt_package.png)

选择合适的配置项后，使用 `pkgs --update` 命令下载软件包并添加到工程中即可。

## 3、使用 paho-mqtt

* 如何从零开始使用，请参考 [用户手册](docs/user-guide.md)。
* 完整的 API 文档，请参考 [API 手册](docs/api.md)。
* 详细的示例介绍，请参考 [示例文档](docs/samples.md) 。
* MQTT 协议工作原理，请参考 [工作原理](docs/principle.md) 。
* 更多**详细介绍文档**位于 [`/docs`](/docs) 文件夹下，**使用软件包进行开发前请务必查看**。

## 4、注意事项

- 正确填写代理服务器的账号密码

    如果账号密码填写错误，MQTT 客户端将无法正确连接到 MQTT 服务器。

- 合理配置 MQTT 线程栈

    如果使用 MQTT TLS 加密连接，MQTT 线程栈至少需要 6144 字节。

## 5、联系方式 & 感谢

* 维护：RT-Thread 开发团队
* 主页：https://github.com/RT-Thread-packages/paho-mqtt
