import {Component} from '@angular/core';
import {
  IMqttMessage,
  IMqttServiceOptions,
  MqttService,
  IPublishOptions,
} from 'ngx-mqtt';
import {MatSnackBar} from '@angular/material/snack-bar';
import {IClientSubscribeOptions} from 'mqtt-browser';
import {Subscription} from 'rxjs';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss'],
})
export class AppComponent {
  constructor(private _mqttService: MqttService, private _snackBar: MatSnackBar) {
    this.client = this._mqttService;
  }

  private curSubscription: Subscription | undefined;
  connection = {
    hostname: 'broker.emqx.io',
    port: 8083,
    path: '/mqtt',
    clean: true, // Retain session
    connectTimeout: 4000, // Timeout
    reconnectPeriod: 4000, // Reconnection interval
    // Authentication information
    clientId: 'mqttx_597046f4',
    username: 'emqx_test',
    password: 'emqx_test',
    protocol: 'ws',
  }
  subscription = {
    topic: 'topictest/browser',
    qos: 0,
  };
  publish = {
    topic: 'topictest/browser',
    qos: 0,
    payload: '{ "msg": "Hello, I am browser." }',
  };
  receiveNews = '';
  qosList = [
    {label: 0, value: 0},
    {label: 1, value: 1},
    {label: 2, value: 2},
  ];
  client: MqttService | undefined;
  isConnection = false;
  subscribeSuccess = false;

  // Create connection
  createConnection() {
    // Connection string, specify the connection method through the protocol
    // ws unencrypted WebSocket connection
    // wss encrypted WebSocket connection
    // mqtt unencrypted TCP connection
    // mqtts encrypted TCP connection
    // wxs WeChat Mini Program connection
    // alis Alipay Mini Program connection
    try {
      this.client?.connect(this.connection as IMqttServiceOptions)
    } catch (error) {
      console.log('mqtt.connect error', error);
    }
    this.client?.onConnect.subscribe(() => {
      this.isConnection = true
      console.log('Connection succeeded!');
    });
    this.client?.onError.subscribe((error: any) => {
      this.isConnection = false
      console.log('Connection failed', error);
    });
    this.client?.onMessage.subscribe((packet: any) => {
      this.receiveNews = this.receiveNews.concat([packet.payload.toString(), '\n'].join())
      console.log(`Received message ${packet.payload.toString()} from topic ${packet.topic}`)
    })
  }

  // Subscribe to topic
  doSubscribe() {
    const {topic, qos} = this.subscription
    if (!this.client) {
      this._snackBar.open('There is no mqtt client available...', 'close');
      return;
    }
    this.curSubscription = this.client.observe(topic, {qos} as IClientSubscribeOptions)
      .subscribe((message: IMqttMessage) => {
        this.subscribeSuccess = true
        const msg = ['Received message: ', message.payload.toString()].join(' ');
        this._snackBar.open(msg, 'close');
        console.log(message);
      });
  }

  // Unsubscribe
  doUnSubscribe() {
    this.curSubscription?.unsubscribe()
    this.subscribeSuccess = false
  }

  // Send message
  doPublish() {
    const {topic, qos, payload} = this.publish
    console.log(this.publish)
    this.client?.unsafePublish(topic, payload, {qos} as IPublishOptions)
  }

  // Disconnect
  destroyConnection() {
    try {
      this.client?.disconnect(true)
      this.isConnection = false
      console.log('Successfully disconnected!')
    } catch (error: any) {
      console.log('Disconnect failed', error.toString())
    }
  }
}
