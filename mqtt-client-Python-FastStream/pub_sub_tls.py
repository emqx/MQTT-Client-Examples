import ssl
from uuid import uuid4

from faststream import FastStream
from faststream.mqtt import MQTTBroker, MQTTMessage, QoS
from faststream.security import SASLPlaintext

BROKER = "broker.emqx.io"
PORT = 8883
TOPIC = "python-faststream/tls"
CLIENT_ID = f"python-faststream-tls-{uuid4().hex[:8]}"
USERNAME = "emqx"
PASSWORD = "public"

ssl_context = ssl.create_default_context()
security = SASLPlaintext(
    username=USERNAME,
    password=PASSWORD,
    ssl_context=ssl_context,
)
broker = MQTTBroker(
    BROKER,
    port=PORT,
    client_id=CLIENT_ID,
    version="5.0",
    security=security,
)
app = FastStream(broker)


@broker.subscriber(TOPIC, qos=QoS.AT_LEAST_ONCE)
async def on_message(payload: dict[str, str], message: MQTTMessage) -> None:
    print(f"Received `{payload}` from topic `{message.raw_message.topic}`")


@app.after_startup
async def publish_demo() -> None:
    payload = {"message": "Hello from FastStream over TLS"}
    await broker.publish(payload, TOPIC, qos=QoS.AT_LEAST_ONCE)
    print(f"Sent `{payload}` to topic `{TOPIC}`")
