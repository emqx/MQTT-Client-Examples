import datetime
import json
import logging
import os
import ssl
import random
import sqlite3
from typing import Dict, Any, List, Optional

import requests
from flask import Flask, jsonify, render_template, g, request
from flask_mqtt import Mqtt


# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

app = Flask(__name__)

DEBUG: bool = False
MQTT_CLIENT: Optional[Mqtt] = None
DATABASE: str = './data/database.db'
SLACK_WEBHOOK: str = 'https://hooks.slack.com/services'
MQTT_TOPIC: str = 'emqx/esp32/telemetry'
app.config['MQTT_BROKER_URL'] = 'broker.emqx.io'
app.config['MQTT_BROKER_PORT'] = 8883
app.config['MQTT_USERNAME'] = ''
app.config['MQTT_PASSWORD'] = ''
app.config['MQTT_KEEPALIVE'] = 60
app.config['MQTT_CLIENT_ID'] = f"flask-mqtt-{random.randint(0, 100000)}"
app.config['MQTT_TLS_ENABLED'] = True
# Load DigiCert Global Root G2, which is used by EMQX Public Broker: broker.emqx.io
app.config['MQTT_TLS_CA_CERTS'] = './certs/broker.emqx.io-ca.crt'
# Load DigiCert Global Root CA ca_cert, which is used by EMQX Cloud Serverless Deployment
# app.config['MQTT_TLS_CA_CERTS'] = './certs/emqxsl-ca.crt'
app.config['MQTT_TLS_VERSION'] = ssl.PROTOCOL_TLSv1_2
app.config['MQTT_RECONNECT_DELAY'] = 3


def init_mqtt(flask_app: Flask) -> None:
    global MQTT_CLIENT
    MQTT_CLIENT = Mqtt(flask_app)

    @MQTT_CLIENT.on_connect()
    def handle_connect(client, userdata, flags, rc):
        client_id = flask_app.config['MQTT_CLIENT_ID']
        mqtt_broker = flask_app.config['MQTT_BROKER_URL']
        if rc == 0:
            logging.info(f"Client {client_id} connected to {mqtt_broker}!")
            MQTT_CLIENT.subscribe(MQTT_TOPIC)
        else:
            logging.error(f"Failed to connect, return code {rc}")

    @MQTT_CLIENT.on_message()
    def handle_mqtt_message(client, userdata, message) -> None:
        try:
            payload: Dict[str, Any] = json.loads(message.payload.decode())
            temp: float = payload['temp']
        except (json.JSONDecodeError, KeyError, TypeError) as e:
            logging.error(f"Error processing message: {e}")
            return
        if temp > 25:
            logging.warning("Temperature too high, sending alert to Slack!")
            requests.post(
                SLACK_WEBHOOK,
                data=json.dumps({'text': f"Temperature too high: {temp}"}),
                headers={'Content-Type': 'application/json'}
            )
        uptime: str = datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S")
        logging.info(f"Received message: {temp} at {uptime}")
        with app.app_context():
            db = get_db()
            db.execute(
                "INSERT INTO temperature_data (uptime, temp) VALUES (?, ?)",
                (uptime, temp)
            )
            db.commit()


def get_db() -> sqlite3.Connection:
    db: sqlite3.Connection = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect(DATABASE)
    return db


@app.teardown_appcontext
def close_connection(exception) -> None:
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()


@app.route('/')
def index():
    redirect_url = request.url_root + 'temp_chart'
    return f"<a href='{redirect_url}'>Go to Temperature chart 😁</a>"


@app.route('/ping')
def ping():
    return "pong"


@app.route('/temp_chart')
def chart():
    return render_template('chart.html')


@app.route('/data')
def data():
    hours: int = int(request.args.get('hours', 1, type=int))
    if hours > 24 or hours < 1:
        hours = 1

    db = get_db()
    min_time: datetime.datetime = datetime.datetime.utcnow() - datetime.timedelta(hours=hours)
    cursor = db.execute(
        "SELECT * FROM temperature_data WHERE uptime > ?",
        (min_time.strftime("%Y-%m-%d %H:%M:%S"),)
    )
    rows = cursor.fetchall()
    times: List[str] = [row[1] for row in rows]
    temperatures: List[float] = [row[2] for row in rows]
    return jsonify({"times": times, "temperatures": temperatures})


if __name__ == '__main__':

    with app.app_context():
        d = get_db()
        d.execute(
            "CREATE TABLE IF NOT EXISTS "
            "temperature_data (id INTEGER PRIMARY KEY, uptime TEXT, temp REAL)"
        )
        d.commit()
    if DEBUG:
        if os.environ.get('WERKZEUG_RUN_MAIN') == 'true':
            init_mqtt(app)
    else:
        init_mqtt(app)

    app.run(host='0.0.0.0', debug=DEBUG, port=8080)
