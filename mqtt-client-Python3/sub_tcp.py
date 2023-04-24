# python 3.x

import logging
import random
import time

from paho.mqtt import client as mqtt_client

BROKER = 'broker.emqx.io'
PORT = 1883
TOPIC = "python-mqtt/tcp"
# generate client ID with pub prefix randomly
CLIENT_ID = "python-mqtt-tcp-sub-{id}".format(id=random.randint(0, 1000))
USERNAME = 'emqx'
PASSWORD = 'public'

FIRST_RECONNECT_DELAY = 1
RECONNECT_RATE = 2
MAX_RECONNECT_COUNT = 12
MAX_RECONNECT_DELAY = 60

FLAG_EXIT = False


def on_connect(client, userdata, flags, rc):
    if rc == 0 and client.is_connected():
        print("Connected to MQTT Broker!")
        client.subscribe(TOPIC)
    else:
        print("Failed to connect, return code {rc}".format(rc=rc), )


def on_disconnect(client, userdata, rc):
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


def on_message(client, userdata, msg):
    print("Received `{payload}` from `{topic}` topic".format(
        payload=msg.payload.decode(), topic=msg.topic))


def connect_mqtt():
    client = mqtt_client.Client(CLIENT_ID)
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER, PORT, keepalive=3)
    client.on_disconnect = on_disconnect
    return client


def run():
    logging.basicConfig(format='%(asctime)s - %(levelname)s: %(message)s',
                        level=logging.DEBUG)
    client = connect_mqtt()
    client.loop_forever()


if __name__ == '__main__':
    run()
