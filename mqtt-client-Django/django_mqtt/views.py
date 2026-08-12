import json
import paho.mqtt.client as mqtt

from django.http import JsonResponse
from django.conf import settings
from django.views.decorators.csrf import csrf_exempt


@csrf_exempt
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

    # Run the network loop so the CONNECT handshake completes before
    # publishing, and wait for the publish to be acknowledged
    client.loop_start()
    msg_info = client.publish(request_data["topic"], request_data["msg"], qos=1)
    try:
        msg_info.wait_for_publish(timeout=5)
    except (RuntimeError, ValueError):
        pass
    client.disconnect()
    client.loop_stop()

    return JsonResponse({"code": msg_info.rc})

