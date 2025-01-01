"""
Install umqtt.simple library:
$ micropython -m mip install umqtt.simple

To run this script:
$ micropython sub.py
"""
import time

from umqtt.simple import MQTTClient


# Define the connection information for the sub client
SERVER = "broker.emqx.io"
ClientID = f'raspberry-sub-{time.time_ns()}'
user = "emqx"
password = "public"
topic = "raspberry/mqtt"
msg = b'{"msg":"hello"}'


def sub(topic, msg):
    # Print the topic and message in the callback function
    print('received message %s on topic %s' % (msg, topic))


def main(server=SERVER):
    # Create a connection, parameters are client ID, broker address, broker port number, authentication information
    client = MQTTClient(ClientID, server, 1883, user, password)
    client.set_callback(sub)
    client.connect()
    print('Connected to MQTT Broker "%s"' % (server))
    # If the connection to the broker is lost and reconnected, continue subscribing to the raspberry/topic
    client.subscribe(topic)
    while True:
        if True:
            client.wait_msg()
        else:
            client.check_msg()
            time.sleep(1)


if __name__ == "__main__":
    main()
