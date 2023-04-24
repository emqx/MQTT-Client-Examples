# python 3.x

import json
import logging
import random
import time

from paho.mqtt import client as mqtt_client

BROKER = 'broker.emqx.io'
PORT = 8883
TOPIC = "python-mqtt/tls"
# generate client ID with pub prefix randomly
CLIENT_ID = "python-mqtt-tls-pub-{id}".format(id=random.randint(0, 1000))
USERNAME = 'emqx'
PASSWORD = 'public'
FLAG_CONNECTED = 0

FIRST_RECONNECT_DELAY = 1
RECONNECT_RATE = 2
MAX_RECONNECT_COUNT = 12
MAX_RECONNECT_DELAY = 60

FLAG_EXIT = False


def on_connect(client, userdata, flags, rc):
    if rc == 0 and client.is_connected():
        print("Connected to MQTT Broker!")
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


def connect_mqtt():
    client = mqtt_client.Client(CLIENT_ID)
    client.tls_set(ca_certs='./broker.emqx.io-ca.crt')
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.connect(BROKER, PORT, keepalive=3)
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
        time.sleep(1)


def run():
    logging.basicConfig(format='%(asctime)s - %(levelname)s: %(message)s',
                        level=logging.DEBUG)
    client = connect_mqtt()
    client.loop_start()
    time.sleep(1)
    if client.is_connected():
        publish(client)
    else:
        client.loop_stop()


if __name__ == '__main__':
    run()
