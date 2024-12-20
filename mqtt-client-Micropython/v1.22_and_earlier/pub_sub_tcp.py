import json
import random
import time
import wifi

from umqtt.simple import MQTTClient

SERVER = "broker.emqx.io"
PORT = 1883
CLIENT_ID = 'micropython-client-{id}'.format(id = random.getrandbits(8))
USERNAME = 'emqx'
PASSWORD = 'public'
TOPIC = "raspberry/mqtt"

def on_message(topic, msg):
    print("Received '{payload}' from topic '{topic}'\n".format(
        payload = msg.decode(), topic = topic.decode()))

def connect():
    client = MQTTClient(CLIENT_ID, SERVER, PORT, USERNAME, PASSWORD)
    client.connect()
    print('Connected to MQTT Broker "{server}"'.format(server = SERVER))
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
        print("Send '{msg}' to topic '{topic}'".format(msg = msg, topic = TOPIC))
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
