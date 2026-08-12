# Python FastStream MQTT client examples

These examples show how to publish and subscribe to MQTT 5.0 messages with
[FastStream](https://faststream.ag2.ai/latest/mqtt/):

- `pub_sub_tcp.py` connects over TCP.
- `pub_sub_tls.py` connects over TLS and verifies the broker certificate.

Both examples use the public EMQX broker at `broker.emqx.io` with the public
`emqx/public` credentials. Do not use this broker for production workloads or
publish sensitive data to it.

## Prerequisites

- Python 3.10 or newer

## Installation

```bash
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r requirements.txt
```

## Run

Start the TCP example:

```bash
faststream run pub_sub_tcp:app
```

Or start the TLS example:

```bash
faststream run pub_sub_tls:app
```

Each application subscribes to its example topic and publishes one message
after startup. Press `Ctrl+C` to stop it.

## Test

The tests use FastStream's in-memory `TestMQTTBroker`, so they do not connect to
the public EMQX broker:

```bash
pytest tests.py
```
