/* @flow */
import React, { Component } from 'react';
import {
  View,
  Text,
  StyleSheet,
  FlatList,
} from 'react-native';

import { Input, Button} from '@rneui/base';
import AsyncStorage from '@react-native-async-storage/async-storage';
import init from 'react_native_mqtt';

init({
  size: 10000,
  storageBackend: AsyncStorage,
  defaultExpires: 1000 * 3600 * 24,
  enableCache: true,
  sync : {}
});
const options = {
  host: 'broker.emqx.io',
  port: 8083,
  path: '/testTopic',
  id: 'id_' + parseInt(Math.random()*100000)
};
// Create client instance
client = new Paho.MQTT.Client(options.host, options.port, options.path);

class App extends Component {
  constructor(props){
    super(props)
    this.state={
      topic: 'testTopic',
      subscribedTopic: '',
      message: '',
      messageList: [],
      status: ''
    };
    client.onConnectionLost = this.onConnectionLost;
    client.onMessageArrived = this.onMessageArrived;
  }
  // Connection successful
  onConnect = () => {
    console.log('onConnect');
    this.setState({ status: 'connected' });
  }
  // Connection failed
  onFailure = (err) => {
    console.log('Connect failed!');
    console.log(err);
    this.setState({ status: 'failed' });
  }
  // Connect to MQTT server
  connect = () => {
    this.setState(
      { status: 'isFetching' },
      () => {
        client.connect({
          onSuccess: this.onConnect,
          useSSL: false,
          timeout: 3,
          onFailure: this.onFailure
        });
      }
    );
  }
  // Connection lost
  onConnectionLost=(responseObject)=>{
    if (responseObject.errorCode !== 0) {
      console.log('onConnectionLost:' + responseObject.errorMessage);
    }
  }
  // Message received
  onMessageArrived = (message )=> {
    console.log('onMessageArrived:' + message.payloadString);
    newmessageList = this.state.messageList;
    newmessageList.unshift(message.payloadString);
    this.setState({ messageList: newmessageList });
    // this.MessageListRef.scrollToEnd({animated: false});
  }
  onChangeTopic = (text) => {
    this.setState({ topic: text });
  }
  // Topic subscription
  subscribeTopic = () => {
    this.setState(
      { subscribedTopic: this.state.topic },
      () => {
        client.subscribe(this.state.subscribedTopic, { qos: 0 });
      }
    );
  }
  // Unsubscribe
  unSubscribeTopic = () => {
    client.unsubscribe(this.state.subscribedTopic);
    this.setState({ subscribedTopic: '' });
  }
  onChangeMessage = (text) => {
    this.setState({ message: text });
  }
  // Message publishing
  sendMessage = () =>{
    var message = new Paho.MQTT.Message(options.id + ':' + this.state.message);
    message.destinationName = this.state.subscribedTopic;
    client.send(message);
  }
  renderRow = ({ item, index }) => {
    idMessage = item.split(':');
    console.log('>>>ITEM', item);
    return(
      <View 
        style={[
          styles.componentMessage,
          idMessage[0] == options.id ?
            styles.myMessageComponent
          :
            (idMessage.length == 1 ? styles.introMessage : styles.messageComponent),
        ]}
      >
        <Text style={idMessage.length == 1 ? styles.textIntro : styles.textMessage}>
          {item}
        </Text>
      </View>
    )
  }
  _keyExtractor = (item, index) => item + index;
  render() {
    const { status, messageList } = this.state;
    return (
      <View style={styles.container}>
        <Text
          style={{
            marginBottom: 50,
            textAlign: 'center',
            color: this.state.status === 'connected' ? 'green' : 'black'
          }}
        >
          ClientID: {options.id}
        </Text>
        {
          this.state.status === 'connected' ?
            <View>
              <Button
                type='solid'
                title='DISCONNECT'
                onPress={() => {
                  client.disconnect();
                  this.setState({ status: '', subscribedTopic: '' });
                }}
                buttonStyle={{ marginBottom:50, backgroundColor: '#397af8' }}
                icon={{ name: 'lan-disconnect', type: 'material-community', color: 'white' }}
              />
              <View style={{ marginBottom: 30, alignItems: 'center' }}>
                <Input
                  label='TOPIC'
                  placeholder=''
                  value={this.state.topic}
                  onChangeText={this.onChangeTopic}
                  disabled={this.state.subscribedTopic}
                />
                {
                  this.state.subscribedTopic ?
                    <Button
                      type='solid'
                      title='UNSUBSCRIBE'
                      onPress={this.unSubscribeTopic}
                      buttonStyle={{ backgroundColor: '#397af8' }}
                      icon={{ name: 'link-variant-off', type: 'material-community', color: 'white' }}
                    />
                  :
                    <Button
                      type='solid'
                      title='SUBSCRIBE'
                      onPress={this.subscribeTopic}
                      buttonStyle={{ backgroundColor: '#397af8' }}
                      icon={{ name: 'link-variant', type: 'material-community', color: 'white' }}
                      disabled={!this.state.topic || this.state.topic.match(/ /) ? true : false}
                    />
                }
              </View>
              {
                this.state.subscribedTopic ?
                  <View style={{ marginBottom: 30, alignItems: 'center' }}>
                    <Input
                      label='MESSAGE'
                      placeholder=''
                      value={this.state.message}
                      onChangeText={this.onChangeMessage}
                    />
                    <Button
                      type='solid'
                      title='PUBLISH'
                      onPress={this.sendMessage}
                      buttonStyle={{ backgroundColor: status === 'failed' ? 'red' : '#397af8' }}
                      icon={{ name: 'send', color: 'white' }}
                      disabled={!this.state.message || this.state.message.match(/^[ ]*$/) ? true : false}
                    />
                  </View>
                :
                  null
              }
            </View>
          :
            <Button
              type='solid'
              title='CONNECT'
              onPress={this.connect}
              buttonStyle={{
                marginBottom:50,
                backgroundColor: status === 'failed' ? 'red' : '#397af8'
              }}
              icon={{ name: 'lan-connect', type: 'material-community', color: 'white' }}
              loading={status === 'isFetching' ? true : false}
              disabled={status === 'isFetching' ? true : false}
            />
        }
        <View style={styles.messageBox}>
          <FlatList
            ref={ref => (this.MessageListRef = ref)}
            data={messageList}
            renderItem={this.renderRow}
            keyExtractor={this._keyExtractor}
            extraData={this.state}
          />
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    paddingTop: 70,
  },
  messageBox:{
    margin: 16,
    flex: 1,
  },
  myMessageComponent:{
    backgroundColor: '#000000',
    borderRadius: 3,
    padding: 5,
    marginBottom: 5,
  },
  messageComponent:{
    marginBottom: 5,
    backgroundColor: '#0075e2',
    padding: 5,
    borderRadius: 3,
  },
  introMessage:{
  },
  textInput:{
    height: 40,
    margin: 5,
    borderWidth: 1,
    padding: 5,
  },
  textIntro:{
    color: 'black',
    fontSize: 12,
  },
  textMessage:{
    color: 'white',
    fontSize: 16,
  },
});

export default App;