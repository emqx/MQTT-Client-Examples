"""
This script code is only available for MicroPython version 1.23 and above.

Install umqtt.simple library:
$ micropython -m mip install umqtt.simple

This script demonstrates how to publish and subscribe to an MQTT broker using no secure connection, don't use this in production.

To run this script:
$ micropython pub_sub_tls.py
"""

import json
import random
import ssl
import time

from umqtt.simple import MQTTClient

import wifi


SERVER = "broker.emqx.io"
PORT = 8883
CLIENT_ID = 'micropython-client-{id}'.format(id=random.getrandbits(8))
USERNAME = 'emqx'
PASSWORD = 'public'
TOPIC = "raspberry/mqtt"


def on_message(topic, msg):
    print("Received '{payload}' from topic '{topic}'\n".format(
        payload=msg.decode(), topic=topic.decode()))


def create_ssl_context():
    # Create an SSL context
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ssl_context.verify_mode = ssl.CERT_NONE
    return ssl_context


def connect():
    ssl_context = create_ssl_context()

    client = MQTTClient(CLIENT_ID, SERVER, PORT, USERNAME, PASSWORD, ssl=ssl_context)
    client.connect()
    print('Connected to MQTT Broker "{server}"'.format(server=SERVER))
    return client


def subscribe(client):
    client.set_callback(on_message)
    client.subscribe(TOPIC)


def loop_publish(client):
    msg_count = 0
    while True:
        msg_dict = {
            'msg': msg_count
        }
        msg = json.dumps(msg_dict)
        result = client.publish(TOPIC, msg)
        print("Send '{msg}' to topic '{topic}'".format(msg=msg, topic=TOPIC))
        client.wait_msg()
        msg_count += 1
        time.sleep(1)


def run():
    wifi.connect()
    client = connect()
    subscribe(client)
    loop_publish(client)


if __name__ == "__main__":
    run()
