# pub-tls.py
import time
import ussl
from umqtt.simple import MQTTClient

server = "broker.emqx.io"
ClientID = f'raspberry-pub-{time.time_ns()}'
user = "emqx"
password = "public"
topic = b'raspberry/mqtt'
msg = b'{"msg":"hello"}'

def connect():
    print('Connected to MQTT Broker "%s"' % server)
    client = MQTTClient(ClientID, server, 8883, user, password, ssl=True, ssl_params={'server_hostname': server})
    try:
        client.connect()
        return client
    except Exception as e:
        print('Failed to connect to MQTT broker:', e)
        raise

def reconnect():
    print('Failed to connect to MQTT broker. Reconnecting...')
    time.sleep(5)
    client = connect()
    return client

try:
    client = connect()
except Exception:
    client = reconnect()

while True:
    try:
        print('Sending message %s on topic %s' % (msg, topic))
        client.publish(topic, msg, qos=0)
        time.sleep(1)
    except Exception as e:
        print('Failed to publish message:', e)
        client = reconnect()