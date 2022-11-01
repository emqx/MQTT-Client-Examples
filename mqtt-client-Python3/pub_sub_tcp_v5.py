# python 3.x

import json
import random
import time

import paho.mqtt.client as mqtt_client
from paho.mqtt.client import MQTTv5

BROKER = 'broker.emqx.io'
PORT = 1883
PROTOCOL = 5
TOPIC = "python-mqtt/tcp"
# generate client ID with pub prefix randomly
CLIENT_ID = "python-mqtt-tcp-pub-sub-{id}".format(id=random.randint(0, 1000))
USERNAME = 'emqx'
PASSWORD = 'public'
FLAG_CONNECTED = 0

# Upon connection, Paho calls the on_connect() function, which can be defined as needed.
# It is the callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc, properties=None):

    global FLAG_CONNECTED
    if str(rc) == "Success":
        FLAG_CONNECTED = 1
        print("Connected to MQTT Broker with status: {rc}".format(rc=rc))
        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        client.subscribe(TOPIC, qos=0, options=None, properties=None)
        print("Subscribed to topic `{topic}`".format(topic=TOPIC))
        print("Expect to receive automated messages every 2 seconds.\n")
    else:
        print("Failed to connect, return code: {rc}".format(rc=rc))

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("Received `{payload}` from topic `{topic}`".format(
        payload=msg.payload.decode(), topic=msg.topic))

def connect_mqtt():
    # Creates an instance of the MQTT client
    client = mqtt_client.Client(client_id=CLIENT_ID, protocol=MQTTv5)
    # Logs in if there is an existing user defined on the broker side
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    # Broker address and port defined on broker side, keep-alive of choice
    client.connect(BROKER, port=PORT, keepalive=60, clean_start="MQTT_CLEAN_START_FIRST_ONLY")
    return client

def publish(client):
    msg_count = 0
    while True:
        msg_dict = {
            'msg': msg_count
        }
        msg = json.dumps(msg_dict)
        result = client.publish(TOPIC, msg)
        result.wait_for_publish(1)
        if result.is_published():
            print("Sent `{msg}` to topic `{topic}`".format(msg=msg, topic=TOPIC))
        else:
            print("Failed to send message to topic {topic}".format(topic=TOPIC))
        msg_count += 1
        time.sleep(2)

def run():
    client = connect_mqtt()
    client.loop_start()
    time.sleep(2)
    if FLAG_CONNECTED:
        publish(client)
    else:
        client.loop_stop()

if __name__ == '__main__':
    run()
