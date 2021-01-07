import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:flutter_mqtt/mqtt_client.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter MQTT Demo',
      theme: ThemeData(
        primarySwatch: Colors.blue,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      home: MyHomePage(title: 'Flutter MQTT Demo'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key key, this.title}) : super(key: key);

  final String title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  MqttClient client;
  var topic = "topic/test";

  void _publish(String message) {
    final builder = MqttClientPayloadBuilder();
    builder.addString('Hello from flutter_client');
    client?.publishMessage(topic, MqttQos.atLeastOnce, builder.payload);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            RaisedButton(
              child: Text('Connect'),
              onPressed: () => {
                connect().then((value) {
                  client = value;
                })
              },
            ),
            RaisedButton(
              child: Text('Subscribe'),
              onPressed: () {
                return {client?.subscribe(topic, MqttQos.atLeastOnce)};
              },
            ),
            RaisedButton(
              child: Text('Publish'),
              onPressed: () => {this._publish('Hello')},
            ),
            RaisedButton(
              child: Text('Unsubscribe'),
              onPressed: () => {client?.unsubscribe(topic)},
            ),
            RaisedButton(
              child: Text('Disconnect'),
              onPressed: () => {client?.disconnect()},
            ),
          ],
        ),
      ),
    );
  }
}
