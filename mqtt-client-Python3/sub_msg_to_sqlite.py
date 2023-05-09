# python 3.x

import datetime
import json
import random
import sqlite3 as db
# import duckdb as db
import time

from paho.mqtt import client as mqtt_client

BROKER = 'broker.emqx.io'
PORT = 8883
TOPIC = "python-mqtt/tls"
# generate client ID with pub prefix randomly
CLIENT_ID = f'python-mqtt-tls-sub-{random.randint(0, 1000)}'
USERNAME = 'emqx'
PASSWORD = 'public'
CA_CERTS = './broker.emqx.io-ca.crt'

TABLE_NAME = 'message'
# create connection to sqlite database
conn = db.connect(database='./message_log.db')

ts = time.time()


def db_init():
    cursor = conn.cursor()
    cursor.execute(f'''
        CREATE TABLE IF NOT EXISTS {TABLE_NAME}
        (publishAt VARCHAR, clientId VARCHAR, topic VARCHAR)
        ''')
    conn.commit()


def save_message(publish_at, client_id, topic, payload=None, db_conn=conn):
    cursor = conn.cursor()
    try:
        cols, vals = [], []
        data = json.loads(payload)
        for key, val in data.items():
            column = f'{key}'.replace(' ', '_')
            cursor.execute(f'PRAGMA table_info({TABLE_NAME})')
            columns = cursor.fetchall()
            # add column if not exist
            if not any(column in col for col in columns):
                cursor.execute(f'ALTER TABLE {TABLE_NAME} ADD COLUMN {column} VARCHAR')
            cols.append(f'{column}')
            vals.append(f'{val}')
        # insert message data into sqlite database
        cursor.execute(f'''
            INSERT INTO {TABLE_NAME} (publishAt, clientId, topic, {','.join(cols)}) 
            VALUES ('{publish_at}', '{client_id}', '{topic}', {','.join(vals)})
            ''')

        conn.commit()
        print("Saved message successfully!")
    except db.Error as e:
        print("Failed to save message.", e)


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
        client.subscribe(TOPIC)
    else:
        print(f'Failed to connect, return code {rc}')


def on_message(client, userdata, msg):
    timestamp = datetime.datetime.fromtimestamp(ts + msg.timestamp)
    publish_at = timestamp.strftime("%Y-%m-%d %H:%M:%S.%f")
    client_id = CLIENT_ID
    topic = msg.topic
    payload = msg.payload.decode()

    save_message(publish_at, client_id, topic, payload, conn)


def connect_mqtt():
    client = mqtt_client.Client(CLIENT_ID)
    client.tls_set(ca_certs=CA_CERTS)
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER, PORT)
    return client


def run():
    db_init()
    client = connect_mqtt()
    client.loop_forever()
    conn.close()


if __name__ == '__main__':
    run()
