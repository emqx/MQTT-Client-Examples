# Flask MQTT 客户端示例

本仓库提供了一个 Flask MQTT 客户端示例，具有自动重连和消息发布功能。它适合初学者学习如何在 Flask 应用程序中集成 MQTT，并处理消息订阅和发布。

## 特点

- 连接到 MQTT 代理。
- 订阅 MQTT 主题。
- 向 MQTT 主题发布消息。
- 在意外断开连接时自动重连。

## 要求

- Python 3
- Flask
- Flask-MQTT

## 安装

克隆仓库：

```bash
git clone https://github.com/emqx/MQTT-Client-Examples.git
cd mqtt-client-Flask
```

安装依赖：

```bash
pip install -r requirements.txt
```

## 运行应用程序

启动应用程序：

```bash
python flask_mqtt_app.py
```

## 使用方法

- MQTT 客户端订阅指定的主题，并记录收到的任何消息。
- 打开首页，会向主题发送一条消息。

## 贡献

欢迎对该项目做出贡献。请开一个问题或提交一个带有您建议的拉取请求。