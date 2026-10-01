# FlowSDK Rust MQTT Examples

Two standalone applications publish and receive one MQTT 5 message over TCP at
QoS 1, using `flowsdk` crate. Each has its own Cargo
manifest and lockfile; run commands from the repository root.

| Example       | Who runs networking and timers?                                 | Start reading                                        |
|---------------|-----------------------------------------------------------------|------------------------------------------------------|
| Ready client  | FlowSDK's worker on the application's Tokio runtime.            | [ready_client/src/main.rs](ready_client/src/main.rs) |
| No-I/O client | The application, using `mio` readiness and monotonic deadlines. | [no_io_client/src/main.rs](no_io_client/src/main.rs) |

Both use the same MQTT engine. Only CLI/configuration and received-payload checks
are shared in [common.rs](common.rs); the client lifecycle and event loop remain
visible in each example. No FFI or generated bindings are involved.

## Prerequisites

- Rust and Cargo;
- An MQTT 5 broker with TCP access, such as Mosquitto. 

## Build and run

Start a local broker in a separate terminal:

```sh
mosquitto -p 1883
```

Build and run the ready client:

```sh
cargo build --locked --manifest-path FlowSDK_Rust/ready_client/Cargo.toml
cargo run --locked --manifest-path FlowSDK_Rust/ready_client/Cargo.toml -- --host localhost --port 1883
```

Build and run the application-owned event loop:

```sh
cargo build --locked --manifest-path FlowSDK_Rust/no_io_client/Cargo.toml
cargo run --locked --manifest-path FlowSDK_Rust/no_io_client/Cargo.toml -- --host localhost --port 1883
```
Each run generates a unique client ID and topic, then prints:

```text
Connected
Subscribed (QoS 1)
Publish acknowledged (PUBACK)
Received matching payload (34 bytes)
Unsubscribed
SUCCESS: published, received, unsubscribed, and disconnected
```
## Configuration

Both programs accept the same options; append `--help` after Cargo's `--` for help.

| Setting                          | Default                                                                                                |
|----------------------------------|--------------------------------------------------------------------------------------------------------|
| `--host`, `--port`               | `localhost`, `1883`; IPv4/IPv6 addresses also work.                                                    |
| `--client-id`, `--topic`         | Unique per run; topic is under `flowsdk/examples/`.                                                    |
| `--payload`                      | `Hello from FlowSDK (MQTT 5, QoS 1)`; at most 16 KiB.                                                  |
| `--timeout`                      | 10 seconds for the whole exchange; ready-client shutdown has a separate deadline of the same duration. |
| `--keep-alive`                   | 15 seconds.                                                                                            |
| `MQTT_USERNAME`, `MQTT_PASSWORD` | Public example credentials `emqx` and `public`. Set both to empty for anonymous access.                |


## How the two modes work

**Ready client:** `TokioAsyncMqttClient` owns the networking worker. Methods such
as `connect_sync()` and `publish_sync()` are still async Rust methods: they wait
for the operation's acknowledgement. Worker callbacks forward events through a
bounded channel without awaiting client calls or the application consumer. 
The application waits for actual delivery and
calls `shutdown()` on both success and failure paths.

**No-I/O client:** `NoIoMqttClient` owns protocol state. The application completes
a nonblocking TCP connection, feeds socket input, handles engine events, writes
outgoing bytes, and schedules ticks using `next_tick_at()`. Its
[output buffer](no_io_client/src/output.rs) retains the unwritten tail after short
writes or `WouldBlock`. Writable interest is removed when output drains. Disconnect
bytes are written before closing; errors notify the engine and release the socket.
Hostname resolution runs synchronously before the event loop. The `--timeout`
deadline starts after this lookup; DNS uses the operating system's timeout.

No Tokio runtime is created in the no-I/O example. FlowSDK still depends on Tokio
utility types; separate packages keep its runtime/networking features out of this
build. The pure-engine test demos that you could feeds fragmented bytes and advances
time without a socket or runtime. 
