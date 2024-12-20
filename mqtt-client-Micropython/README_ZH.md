# MicroPython MQTT 客户端使用示例

在本示例中，我们提供了使用 TCP 和 WebSocket 协议连接到 MQTT Broker 的示例代码。代码兼容不同版本的 MicroPython。请根据您的版本参考相应的文件夹：

- **`v1.22_and_earlier`**：包含适用于 MicroPython 1.22 及更早版本的示例。
- **`v1.23_and_above`**：包含适用于 MicroPython 1.23 及以上版本的示例。
有关 MicroPython MQTT 客户端更多使用文档，请参阅 [MicroPython 官方文档](https://mpython.readthedocs.io/en/master/library/mPython/umqtt.simple.html)。

## 前提

* 本示例中树莓派操作系统为 Raspberry Pi OS with desktop (Debian version: 10，64-bit)

## 安装 MicroPython

```bash
 # 当树莓派使用的操作系统基于 Debian version: 10 时可直接使用如下命令安装 MicroPython
 sudo apt-get update
 sudo apt-get -y install micropython

 # 使用 snap 方式进行安装（推荐）
 sudo apt update
 sudo apt install snapd
 sudo reboot
 sudo snap install core
 sudo snap install micropython

# 源码编译的方式进行安装
参考链接：https://docs.micropython.org/en/latest/develop/gettingstarted.html
```

## 安装依赖库

为了方便连接到 MQTT 服务器，我们需要安装 umqtt.simple 库。

```bash
# 对于 MicroPython 1.22 及更早版本
micropython -m upip install umqtt.simple

# 对于 MicroPython 1.23 及以上版本
micropython -m mip install umqtt.simple
```

## 运行

```bash
micropython sub.py
micropython pub.py
```

## 参阅

[如何在 MicroPython 中使用 MQTT](https://www.emqx.com/zh/blog/micro-MicroPython-mqtt-tutorial-based-on-raspberry-pi)
