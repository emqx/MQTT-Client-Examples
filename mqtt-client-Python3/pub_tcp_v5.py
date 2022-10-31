# python 3.x

import json
import random
import time

from paho.mqtt import client as mqtt_client
from paho.mqtt.client import MQTTv5


BROKER = '127.0.0.1'
PORT = 1883
TOPIC = "python-mqtt/tcp"
# generate client ID with pub prefix randomly
CLIENT_ID = "python-mqtt-tcp-pub-{id}".format(id=random.randint(0, 1000))
USERNAME = 'emqx'
PASSWORD = 'public'
FLAG_CONNECTED = 0


def on_connect(client, userdata, flags, rc, properties=None):
    global FLAG_CONNECTED
    if rc == 0:
        FLAG_CONNECTED = 1
        print("Connected to MQTT Broker!")
    else:
        print("Failed to connect, return code {rc}".format(rc=rc), )


def connect_mqtt():
    # client = mqtt_client.Client(CLIENT_ID)
    client = mqtt_client.Client(CLIENT_ID, protocol=MQTTv5)
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.connect(BROKER, PORT)
    return client


def publish(client):
    msg_count = 0
    while True:
        msg_dict = {
            'msg': msg_count
        }
        msg = json.dumps(msg_dict)
        result = client.publish(TOPIC, msg, qos=0)
        result.wait_for_publish(1)
        if result.is_published():
            print("Send `{msg}` to topic `{topic}`".format(msg=msg, topic=TOPIC))
        else:
            print("Failed to send message to topic {topic}".format(topic=TOPIC))
        msg_count += 1
        time.sleep(1)


def run():
    client = connect_mqtt()
    client.loop_start()
    time.sleep(1)
    if FLAG_CONNECTED:
        publish(client)
    else:
        client.loop_stop()


if __name__ == '__main__':
    run()
