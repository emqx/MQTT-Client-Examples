# Elixir MQTT 客户端示例
在此示例中，我们提供了 [Elixir](https://elixir-lang.org/) 订阅主题和发送消息的示例代码。
我们使用 [`emqtt`](https://github.com/emqx/emqtt) 客户端。 有关更多文档，请参阅 [`emqtt` 文档](https://github.com/emqx/emqtt/blob/master/README.md)。

我们演示
* 具有不同 QOS 的基本发布/订阅操作；
* 通过 TLS 安全连接到代理的选项。

## 运行
执行以下命令

```bash
elixir mqtt.exs
elixir mqtt_tls.exs
```

在运行脚本之前，Elixir 可能会请求安装 Hex 客户端的权限。
