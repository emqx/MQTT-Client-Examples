# Elixir MQTT client examples
In this example we provide example code for [Elixir](https://elixir-lang.org/) to subscribe to topics and send messages.
We use [`emqtt`](https://github.com/emqx/emqtt) client. For more documentation, see [`emqtt` documentation](https://github.com/emqx/emqtt/blob/master/README.md).

We demonstrate
* basic publish/subscribe operations with different QOS;
* options to connect to the broker securely via TLS.

## Run
Execute the following commands

```bash
elixir mqtt.exs
elixir mqtt_tls.exs
```

Elixir may ask permission to install Hex client before running the scripts.
