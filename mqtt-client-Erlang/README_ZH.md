# mqttex (MQTT Examples)

MQTT 客户端示例程序，基于 [emqtt](https://github.com/emqx/emqtt)

- [x] 包含 TCP/TLS/WS/WSS 连接示例。
- [x] 发布/订阅示例。
- [x] 异步发布示例。
- [x] 消息接收示例。

## 运行示例程序

1. 首先，使用 rebar3 编译工程。
```
rebar3 compile
```

2. 启动应用。

```
rebar3 shell
```

3. 运行基于 TCP 的示例程序, 例如:
```
1> mqttex:run(tcp).
publish msg[1] success!
publish msg[2] success!
publish msg[3] success!
async publish msg[1] success
async publish msg[2] success
async publish msg[3] success
subscribe/unsubscribe success
publisher publish success
received message topic: <<"testtopic/1">>, payload: <<"123456">>
ok
2>
```

当然，也可以通过执行 `mqttex:run(tls)` 或 `mqttex:run(ws)` 或 `mqttex:run(wss)` 来运行
基于其它通信连接层的示例程序。
