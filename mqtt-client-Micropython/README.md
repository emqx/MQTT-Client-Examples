# MicroPython MQTT client examples

In this example, we provide sample codes for connecting TCP and Websocket protocols to MQTT Broker.
For more documentation on using the MicroPython MQTT client, please refer to [MicroPython Documentation](https://mpython.readthedocs.io/en/master/library/mPython/umqtt.simple.html)

## Prerequisites

* In this example, the OS is Raspberry Pi OS with desktop (Debian version: 10, 64-bit)

## Install MicroPython

```bash
# When the operating system used by the Raspberry Pi is based on Debian version: 10, you can directly use the following command to install MicroPython sudo apt-get update
 sudo apt-get -y install micropython

 # Install using snap (recommended)
 sudo apt update
 sudo apt install snapd
 sudo reboot
 sudo snap install core
 sudo snap install micropython

# Install by source code compilation
https://docs.micropython.org/en/latest/develop/gettingstarted.html
```

## Install dependent libraries

In order to connect to the MQTT server conveniently, we need to install the umqtt.simple library.

```bash
micropython -m upip install umqtt.simple
```

## Run

```bash
micropython sub.py
micropython pub.py
```

## Other

[MicroPython MQTT Tutorial Based on Raspberry Pi](https://www.emqx.com/zh/blog/micro-MicroPython-mqtt-tutorial-based-on-raspberry-pi)
