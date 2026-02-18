import paho.mqtt.client as mqtt
from django.conf import settings


def on_connect(mqtt_client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("Connected successfully")
        mqtt_client.subscribe("django/mqtt")
    else:
        print("Bad connection. Code:", reason_code)


def on_message(mqtt_client, userdata, msg):
    print(f"Received message on topic: {msg.topic} with payload: {msg.payload}")


def create_mqtt_client():
    """Create and return an MQTT client configured for connection"""
    client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message
    
    # Only set credentials if they are provided
    if settings.MQTT_USER and settings.MQTT_PASSWORD:
        client.username_pw_set(settings.MQTT_USER, settings.MQTT_PASSWORD)
    
    client.connect(
        host=settings.MQTT_SERVER,
        port=settings.MQTT_PORT,
        keepalive=settings.MQTT_KEEPALIVE
    )
    
    return client

