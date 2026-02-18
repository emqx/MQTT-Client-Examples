# Spring Boot MQTT Client Example

This is a Spring Boot application that demonstrates MQTT client functionality.

## Features
- Connect to MQTT broker
- Subscribe to topics
- Publish messages
- Handle incoming messages

## Prerequisites
- Java 17+
- Maven

## Configuration
Edit application.properties to configure your MQTT broker connection:

- mqtt.broker-url: MQTT broker URL (default: tcp://broker.emqx.io:1883)
- mqtt.client-id: Unique client identifier
- mqtt.username: Username for authentication
- mqtt.password: Password for authentication

## How to Run
1. Clone the repository
2. Navigate to the project directory
3. Run: \`mvn spring-boot:run\`
4. The application will start on port 8080

## API Endpoints
- POST /mqtt/connect - Connect to MQTT broker
- POST /mqtt/disconnect - Disconnect from MQTT broker
- POST /mqtt/subscribe?topic={topic} - Subscribe to a topic
- POST /mqtt/publish?topic={topic}&message={message} - Publish a message

## Example Usage
Connect to broker:
curl -X POST http://localhost:8080/mqtt/connect

Publish a message:
curl -X POST "http://localhost:8080/mqtt/publish?topic=test/topic&message=Hello%20World"

Subscribe to a topic:
curl -X POST "http://localhost:8080/mqtt/subscribe?topic=test/topic"
