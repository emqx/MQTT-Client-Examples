import json
import paho.mqtt.client as mqtt

from django.http import JsonResponse
from django.conf import settings


def publish_message(request):
    request_data = json.loads(request.body)
    
    # Create a temporary client for publishing
    client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
    
    # Only set credentials if they are provided
    if settings.MQTT_USER and settings.MQTT_PASSWORD:
        client.username_pw_set(settings.MQTT_USER, settings.MQTT_PASSWORD)
    
    client.connect(
        host=settings.MQTT_SERVER,
        port=settings.MQTT_PORT,
        keepalive=settings.MQTT_KEEPALIVE
    )
    
    rc, mid = client.publish(request_data["topic"], request_data["msg"])
    client.disconnect()
    
    return JsonResponse({"code": rc})

