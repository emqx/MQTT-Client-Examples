"""
Install umqtt.simple library:
$ micropython -m mip install umqtt.simple

To run this script:
$ micropython pub_sub_tls.py
"""
import time

from umqtt.simple import MQTTClient


# Define the connection information for the pub client
server = "broker.emqx.io"
ClientID = f'raspberry-pub-{time.time_ns()}'
user = "emqx"
password = "public"
topic = "raspberry/mqtt"
msg = b'{"msg":"hello"}'


# Create a connection, parameters are client ID, broker address, broker port number, authentication information
def connect():
    print('Connected to MQTT Broker "%s"' % (server))
    client = MQTTClient(ClientID, server, 1883, user, password)
    client.connect()
    return client


def reconnect():
    # If unable to connect to the broker, print a message to notify that the connection failed and wait 5 seconds to reconnect
    print('Failed to connect to MQTT broker, Reconnecting...' % (server))
    time.sleep(5)
    client.reconnect()


# If able to connect to the broker, call connect(), otherwise call reconnect()
try:
    client = connect()
except OSError as e:
    reconnect()

# Send a message to the topic raspberry/mqtt every second
while True:
    print('send message %s on topic %s' % (msg, topic))
    client.publish(topic, msg, qos=0)
    time.sleep(1)
