#!/bin/env python3
# -*- coding: utf-8 -*-

# This is an example of how to use the MQTT client library to send a file to
# the broker using the EMQX File Transfer feature.
#
# The EMQX File Transfer documentation is available here:
# https://www.emqx.io/docs/en/v5/file-transfer/introduction.html
#
# This example reads a file from the file system and publishes it to the
# broker (the file transfer feature has to be enabled in the EMQX
# configuration). The user can specify the file path, file id, file name etc
# as command line parameters. Run the program with the --help flag to see the
# list of options.
#
# Note that this example does not handle PUBACK Reason Codes the broker sends
# in response to the messages, due to limitations of the client library.
# Hence, there's no proper error handling as it's not possible to tell if
# some file transfer operation was successful or not.

import argparse
from collections import namedtuple
import logging
import os
import json
import hashlib
import paho.mqtt.client as mqtt

BROKER = 'broker.emqx.io'
PORT = 1883
CLIENTID = 'python-mqtt-file-transfer-1'

UserData = namedtuple(
    "UserData",
    ["file", "file_id", "meta", "segment_size", "stages", "state"])


def process_transfer(client, userdata, stage):
    logging.info(f"Processing transfer stage '{stage}' ...")
    file_id = userdata.file_id
    match stage:
        case "init":
            # Publish the metadata
            meta = json.dumps(userdata.meta)
            pub = client.publish(f"$file/{file_id}/init", payload=meta, qos=1)
            userdata.stages[pub.mid] = "segments"

        case "segments":
            # Publish the file segments
            sent = 0
            ssize = userdata.segment_size
            hasher = hashlib.new('sha256')
            segment = userdata.file.read(ssize)
            while segment:
                hasher.update(segment)
                pub = client.publish(f"$file/{file_id}/{sent}", payload=segment, qos=1)
                sent += len(segment)
                segment = userdata.file.read(ssize)
            userdata.state["sent"] = sent
            userdata.state["hasher"] = hasher
            userdata.stages[pub.mid] = "fin"

        case "fin":
            # Publish the fin packet
            sent = userdata.state["sent"]
            hasher = userdata.state["hasher"]
            pub = client.publish(f"$file/{file_id}/fin/{sent}/{hasher.hexdigest()}", qos=1)
            userdata.stages[pub.mid] = "close"

        case "close":
            userdata.file.close()
            client.disconnect()


def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0 and client.is_connected():
        logging.info("Connected to the MQTT Broker")
        # If the initial connection is successful, start the file transfer.
        # Otherwise, let the client retransmit outstanding messages and complete
        # the transfer.
        if not userdata.stages:
            process_transfer(client, userdata, "init")
    else:
        logging.error(f'Failed to connect, return code {rc}')


def on_publish(client, userdata, mid):
    logging.info(f"Message {mid} published")
    if mid in userdata.stages:
        process_transfer(client, userdata, userdata.stages[mid])


def run():
    # Setup logging
    logging.basicConfig(format='%(asctime)s - %(levelname)s: %(message)s', level=logging.DEBUG)

    parser = argparse.ArgumentParser(description='File transfer utility')

    # MQTT options
    parser.add_argument("--client-id", default=CLIENTID, type=str, help="MQTT Client ID")
    parser.add_argument("--host", default=BROKER, type=str, help="MQTT Broker host")
    parser.add_argument("--port", default=PORT, type=int, help="MQTT Broker port")

    # File transfer options
    parser.add_argument("--file-id", required=True, type=str,
        help="ID of the transfer")
    parser.add_argument("--file", required=True, type=argparse.FileType("rb"),
        help="Filename of the file to transfer")
    parser.add_argument("--file-name", required=False, type=str,
        help="Name of the file to tell the Broker, base name of --file if not specified")
    parser.add_argument("--segment-size", default=1024, type=int,
        help="Size of each segment to send")

    # Parse arguments and exit if there are any errors
    args = parser.parse_args()

    print(args)

    # Populate the metadata
    meta = {
        "name": args.file_name if args.file_name else os.path.basename(args.file.name),
        "size": os.stat(args.file.fileno()).st_size
    }

    # Create the MQTT client
    client = mqtt.Client(
        client_id=args.client_id,
        protocol=mqtt.MQTTv5,
        transport="tcp",
        userdata=UserData(
            args.file,
            args.file_id,
            meta,
            args.segment_size,
            stages={},
            state={}))
    client.enable_logger(logging.getLogger())
    client.on_connect = on_connect
    client.on_publish = on_publish
    client.connect(args.host, args.port, keepalive=120)

    client.loop_forever()


if __name__ == '__main__':
    run()
