Mix.install([
  {:emqtt, github: "emqx/emqtt", tag: "1.8.3"},
  {:castore, "~> 1.0.0"}
])

require Logger

Process.flag(:trap_exit, true)

## Connect via SSL and verify the server certificate

{:ok, subscriber} = :emqtt.start_link(
  host: "broker.emqx.io",
  port: 8883,
  client_id: "elixir_subscriber",
  username: "user",
  password: "password",
  ssl: true,
  ## https://www.erlang.org/doc/man/ssl.html#type-tls_client_option
  ssl_opts: [
    verify: :verify_peer,
    server_name_indication: 'broker.emqx.io',
    cacertfile: CAStore.file_path()
  ]
)
{:ok, _} = :emqtt.connect(subscriber)

Logger.info("Successfully connected to broker via SSL")

:ok = :emqtt.disconnect(subscriber)

## Connect via SSL and verify the server certificate, but with an invalid hostname

{:ok, subscriber} = :emqtt.start_link(
  host: "broker.emqx.io",
  port: 8883,
  client_id: "elixir_subscriber",
  username: "user",
  password: "password",
  ssl: true,
  ## https://www.erlang.org/doc/man/ssl.html#type-tls_client_option
  ssl_opts: [
    verify: :verify_peer,
    server_name_indication: 'invalid-hostname-broker-emqx.io',
    cacertfile: CAStore.file_path()
  ]
)

## This should fail
{:error, reason} = :emqtt.connect(subscriber)

Logger.error("Failed to connect: #{inspect(reason)}")
