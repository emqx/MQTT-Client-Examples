# Flask MQTT Client Example

This repository provides a Flask MQTT Client example, featuring automatic reconnect and message
publish functionalities. It's designed for beginners to learn about MQTT integration in Flask
applications, handling both message subscriptions and publishing.

## Features

- Connect to MQTT Broker.
- Subscribe to MQTT topics.
- Publish messages to MQTT topics.
- Automatically reconnect on unexpected disconnection.

## Requirements

- Python 3
- Flask
- Flask-MQTT

## Installation

Clone the repository:

```bash
git clone https://github.com/emqx/MQTT-Client-Examples.git
cd mqtt-client-Flask
```

Install dependencies:

```bash
pip install -r requirements.txt
```

## Running the Application

Start the application:

```bash
python flask_mqtt_app.py
```

## Usage

- The MQTT client subscribes to a specified topic and logs any received messages.
- Open the homepage, which publishes a message to the topic.

## Contributing

Contributions to this project are welcome. Please open an issue or submit a pull request with your
suggestions.
