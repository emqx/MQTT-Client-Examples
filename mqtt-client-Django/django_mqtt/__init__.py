from . import mqtt

# Create and start the MQTT client
client = mqtt.create_mqtt_client()
client.loop_start()

