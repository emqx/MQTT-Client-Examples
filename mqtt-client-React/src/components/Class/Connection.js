import React from "react";
import { Card, Button, Form, Input, Row, Col } from "antd";

class Connection extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      record: {
        host: "broker.emqx.io",
        clientId: `mqttjs_ + ${Math.random().toString(16).substr(2, 8)}`,
        port: 8083,
      },
    };
  }

  onRecordChange = (value) => {
    const { record } = this.state;
    const changedRecord = Object.assign(record, value);
    this.setState({ record: changedRecord });
  };

  handleConnect = () => {
    const { host, clientId, port, username, password } = this.state.record;
    const url = `ws://${host}:${port}/mqtt`;
    const options = {
      keepalive: 30,
      protocolId: "MQTT",
      protocolVersion: 4,
      clean: true,
      reconnectPeriod: 1000,
      connectTimeout: 30 * 1000,
      will: {
        topic: "WillMsg",
        payload: "Connection Closed abnormally..!",
        qos: 0,
        retain: false,
      },
      rejectUnauthorized: false,
    };
    options.clientId = clientId;
    options.username = username;
    options.password = password;
    this.props.connect(url, options);
  };

  render() {
    const ConnectionForm = (
      <Form
        layout="vertical"
        name="basic"
        initialValues={this.state.record}
        onValuesChange={this.onRecordChange}
      >
        <Row gutter={20}>
          <Col span={8}>
            <Form.Item label="Host" name="host">
              <Input />
            </Form.Item>
          </Col>
          <Col span={8}>
            <Form.Item label="Port" name="port">
              <Input />
            </Form.Item>
          </Col>
          <Col span={8}>
            <Form.Item label="Client ID" name="clientId">
              <Input />
            </Form.Item>
          </Col>
          <Col span={8}>
            <Form.Item label="Username" name="username">
              <Input />
            </Form.Item>
          </Col>
          <Col span={8}>
            <Form.Item label="Password" name="password">
              <Input />
            </Form.Item>
          </Col>
        </Row>
      </Form>
    );

    return (
      <Card
        title="Connection"
        actions={[
          <Button type="primary" onClick={this.handleConnect}>
            {this.props.connectBtn}
          </Button>,
          <Button danger onClick={this.props.disconnect}>
            Disconnect
          </Button>,
        ]}
      >
        {ConnectionForm}
      </Card>
    );
  }
}

export default Connection;
