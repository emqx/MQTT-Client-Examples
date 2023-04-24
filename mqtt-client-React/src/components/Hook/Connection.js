import React from 'react'
import { Card, Button, Form, Input, Row, Col, Select } from 'antd'

/**
 * this demo uses EMQX Public MQTT Broker (https://www.emqx.com/en/mqtt/public-mqtt5-broker), here are the details:
 *
 * Broker host: broker.emqx.io
 * WebSocket port: 8083
 * WebSocket over TLS/SSL port: 8084
 */

const Connection = ({ connect, disconnect, connectBtn }) => {
  const [form] = Form.useForm()
  const initialConnectionOptions = {
    // ws or wss
    protocol: 'ws',
    host: 'broker.emqx.io',
    clientId: 'emqx_react_' + Math.random().toString(16).substring(2, 8),
    // ws -> 8083; wss -> 8084
    port: 8083,
    /**
     * By default, EMQX allows clients to connect without authentication.
     * https://docs.emqx.com/en/enterprise/v4.4/advanced/auth.html#anonymous-login
     */
    username: 'emqx_test',
    password: 'emqx_test',
  }

  const handleProtocolChange = (value) => {
    form.setFieldsValue({
      port: value === 'wss' ? 8084 : 8083,
    })
  }

  const onFinish = (values) => {
    const { protocol, host, clientId, port, username, password } = values
    const url = `${protocol}://${host}:${port}/mqtt`
    const options = {
      clientId,
      username,
      password,
      clean: true,
      reconnectPeriod: 1000, // ms
      connectTimeout: 30 * 1000, // ms
    }
    connect(url, options)
  }

  const handleConnect = () => {
    form.submit()
  }

  const handleDisconnect = () => {
    disconnect()
  }

  const ConnectionForm = (
    <Form
      layout="vertical"
      name="basic"
      form={form}
      initialValues={initialConnectionOptions}
      onFinish={onFinish}
    >
      <Row gutter={20}>
        <Col span={8}>
          <Form.Item label="Protocol" name="protocol">
            <Select onChange={handleProtocolChange}>
              <Select.Option value="ws">ws</Select.Option>
              <Select.Option value="wss">wss</Select.Option>
            </Select>
          </Form.Item>
        </Col>
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
  )

  return (
    <Card
      title="Connection"
      actions={[
        <Button type="primary" onClick={handleConnect}>
          {connectBtn}
        </Button>,
        <Button danger onClick={handleDisconnect}>
          Disconnect
        </Button>,
      ]}
    >
      {ConnectionForm}
    </Card>
  )
}

export default Connection
