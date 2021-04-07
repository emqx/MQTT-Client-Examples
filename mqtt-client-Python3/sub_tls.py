# python 3.x

import random

from paho.mqtt import client as mqtt_client


BROKER = 'broker.emqx.io'
PORT = 8883
TOPIC = "python-mqtt/tls"
# generate client ID with pub prefix randomly
CLIENT_ID = "python-mqtt-tls-sub-{id}".format(id=random.randint(0, 1000))
USERNAME = 'emqx'
PASSWORD = 'public'


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
        client.subscribe(TOPIC)
    else:
        print("Failed to connect, return code {rc}".format(rc=rc), )


def on_message(client, userdata, msg):
    print("Received `{payload}` from `{topic}` topic".format(
        payload=msg.payload.decode(), topic=msg.topic))


def connect_mqtt():
    client = mqtt_client.Client(CLIENT_ID)
    client.tls_set(ca_certs='./broker.emqx.io-ca.crt')
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER, PORT)
    return client


def run():
    client = connect_mqtt()
    client.loop_forever()


if __name__ == '__main__':
    run()
