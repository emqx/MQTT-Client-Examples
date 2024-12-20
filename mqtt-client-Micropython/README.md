# MicroPython MQTT Client Examples

In this example, we provide sample code for connecting to an MQTT Broker using both TCP and WebSocket protocols. The code is compatible with different versions of MicroPython. Please refer to the appropriate folder for your version:

- **`v1.22_and_earlier`**: Contains examples for MicroPython version 1.22 and earlier.
- **`v1.23_and_above`**: Contains examples for MicroPython version 1.23 and above.


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
# For MicroPython v1.22 and earlier
micropython -m upip install umqtt.simple

# For MicroPython v1.23 and above
micropython -m mip install umqtt.simple
```

## Run

```bash
micropython sub.py
micropython pub.py
```

## Other

[MicroPython MQTT Tutorial Based on Raspberry Pi](https://www.emqx.com/zh/blog/micro-MicroPython-mqtt-tutorial-based-on-raspberry-pi)
