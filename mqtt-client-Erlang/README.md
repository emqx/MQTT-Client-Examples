# mqttex (MQTT Examples)

MQTT client examples based [emqtt](https://github.com/emqx/emqtt)

- [x] Connecting via TCP/TLS/WS/WSS examples.
- [x] Publish/Subscribe examples.
- [x] Async publish examples.
- [x] Receiving messages example.

## Run Example

1. Compile it first
```
rebar3 compile
```

2. Start application

```
rebar3 shell
```

3. Run the TCP examples, i.e:
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

Of course, you can also run example programs based on other communication layers by executing `mqttex:run(tls)` or `mqttex:run(ws)` or `mqttex:run(wss)`.
