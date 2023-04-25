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
CLIENT_ID = "python-mqtt-tls-sub-{id}".format(id=random.randint(0, 1000))
USERNAME = 'emqx'
PASSWORD = 'public'
CA_CERTS = './broker.emqx.io-ca.crt'

TABLE_NAME = 'message'
# create connection to sqlite database
conn = db.connect(database='./message_log.db')

ts = time.time()


def db_init():
    cursor = conn.cursor()
    cursor.execute(
        "CREATE TABLE IF NOT EXISTS {table} "
        "(publishAt VARCHAR, clientId VARCHAR, topic VARCHAR)".
        format(table=TABLE_NAME))
    conn.commit()


def save_message(publish_at, client_id, topic, payload=None, conn=conn):
    cursor = conn.cursor()
    try:
        cols, vals = [], []
        data = json.loads(payload)
        for key, val in data.items():
            column = str(key).replace(' ', '_')
            cursor.execute("PRAGMA table_info({table})".format(table=TABLE_NAME))
            columns = cursor.fetchall()
            # add column if not exist
            if not any(column in col for col in columns):
                cursor.execute(
                    "ALTER TABLE {table} ADD COLUMN {column} VARCHAR".
                    format(table=TABLE_NAME, column=column))
            cols.append(str(column))
            vals.append(f"'{val}'".format(val=val))
        # insert message data into sqlite database
        cursor.execute("INSERT INTO {table} "
                       "(publishAt, clientId, topic, {cols})"
                       "VALUES (?, ?, ?,{vals})".format(table=TABLE_NAME, cols=','.join(cols), vals=','.join(vals)),
                       (publish_at, client_id, topic))
        conn.commit()
        print("Saved message successfully!")
    except db.Error as e:
        print("Failed to save message.", e)


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
        client.subscribe(TOPIC)
    else:
        print("Failed to connect, return code {rc}".format(rc=rc), )


def on_message(client, userdata, msg):
    dt = datetime.datetime.fromtimestamp(ts + msg.timestamp)
    publish_at = dt.strftime("%Y-%m-%d %H:%M:%S.%f")
    client_id = client._client_id.decode()
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
