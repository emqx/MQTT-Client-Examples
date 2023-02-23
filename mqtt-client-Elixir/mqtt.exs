Mix.install([
  {:emqtt, github: "emqx/emqtt", tag: "1.8.3"}
])

require Logger

Process.flag(:trap_exit, true)

topic = "topic-#{:rand.uniform(9999999)}-#{:erlang.system_time(:millisecond)}"

{:ok, subscriber} = :emqtt.start_link(
  host: "broker.emqx.io",
  port: 1883,
  client_id: "elixir_subscriber",
  username: "user",
  password: "password"
)
{:ok, _} = :emqtt.connect(subscriber)

qos1_granted = 1
{:ok, %{} = _props, [^qos1_granted]} = :emqtt.subscribe(subscriber, topic, :qos1)

{:ok, publisher} = :emqtt.start_link(
  host: "broker.emqx.io",
  port: 1883,
  client_id: "elixir_publisher",
  username: "user",
  password: "password"
)
{:ok, _} = :emqtt.connect(publisher)

:ok = :emqtt.publish(publisher, topic, "qos0_message", :qos0)

{:ok, %{reason_code: 0}} = :emqtt.publish(publisher, topic, "qos1_message", :qos1)

{:ok, %{reason_code: 0}} = :emqtt.publish(publisher, topic, "qos2_message", :qos2)

for _ <- 1..3 do
  receive do
    msg ->
      Logger.info("Received message: #{inspect(msg)}")
  after
    1000 ->
      Logger.error("Message not received")
      Logger.flush()
      System.halt(1)
  end
end

:emqtt.disconnect(subscriber)
:emqtt.disconnect(publisher)
