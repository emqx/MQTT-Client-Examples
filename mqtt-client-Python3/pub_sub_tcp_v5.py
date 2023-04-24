# python 3.x

import json
import logging
import random
import time

from paho.mqtt import client as mqtt_client
from paho.mqtt.client import MQTTv5

BROKER = 'broker.emqx.io'
PORT = 1883
PROTOCOL = 5
TOPIC = "python-mqtt/tcp"
# generate client ID with pub prefix randomly
CLIENT_ID = "python-mqtt-tcp-pub-sub-{id}".format(id=random.randint(0, 1000))
USERNAME = 'emqx'
PASSWORD = 'public'

FIRST_RECONNECT_DELAY = 1
RECONNECT_RATE = 2
MAX_RECONNECT_COUNT = 12
MAX_RECONNECT_DELAY = 60

FLAG_EXIT = False


# Upon connection, Paho calls the on_connect() function, which can be defined as needed.
# It is the callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc, properties=None):
    if str(rc) == "Success" and client.is_connected():
        print("Connected to MQTT Broker with status: {rc}".format(rc=rc))
        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        client.subscribe(TOPIC, qos=0, options=None, properties=None)
        print("Subscribed to topic `{topic}`".format(topic=TOPIC))
        print("Expect to receive automated messages every 2 seconds.\n")
    else:
        print("Failed to connect, return code: {rc}".format(rc=rc))


def on_disconnect(client, userdata, rc, properties=None):
    logging.info("Disconnected with result code: " + str(rc))
    reconnect_count, reconnect_delay = 0, FIRST_RECONNECT_DELAY
    while reconnect_count < MAX_RECONNECT_COUNT:
        logging.info("Reconnecting in {reconnect_delay} seconds..."
                     .format(reconnect_delay=reconnect_delay))
        time.sleep(reconnect_delay)

        try:
            client.reconnect()
            logging.info("Reconnected successfully!")
            return
        except Exception as err:
            logging.error("{err}. Reconnect failed. Retrying...".format(err=err))

        reconnect_delay *= RECONNECT_RATE
        if reconnect_delay > MAX_RECONNECT_DELAY:
            reconnect_delay = MAX_RECONNECT_DELAY
        reconnect_count += 1
    logging.info("Reconnect failed after {reconnect_count} attempts. Exiting..."
                 .format(reconnect_count=reconnect_count))
    global FLAG_EXIT
    FLAG_EXIT = True


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
    client.connect(BROKER, port=PORT, keepalive=3, clean_start="MQTT_CLEAN_START_FIRST_ONLY")
    client.on_disconnect = on_disconnect
    return client


def publish(client):
    msg_count = 0
    while not FLAG_EXIT:
        msg_dict = {
            'msg': msg_count
        }
        msg = json.dumps(msg_dict)
        if not client.is_connected():
            logging.error("publish: MQTT client is not connected!")
            time.sleep(1)
            continue
        result = client.publish(TOPIC, msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            print("Send `{msg}` to topic `{topic}`".format(msg=msg, topic=TOPIC))
        else:
            print("Failed to send message to topic {topic}".format(topic=TOPIC))
        msg_count += 1
        time.sleep(2)


def run():
    logging.basicConfig(format='%(asctime)s - %(levelname)s: %(message)s',
                        level=logging.DEBUG)
    client = connect_mqtt()
    client.loop_start()
    time.sleep(2)
    if client.is_connected():
        publish(client)
    else:
        client.loop_stop()


if __name__ == '__main__':
    run()
