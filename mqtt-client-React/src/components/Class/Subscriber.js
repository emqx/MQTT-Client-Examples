import React from "react";
import { Card, Form, Input, Row, Col, Button, Select } from "antd";
import { QosOption } from "./index";

class Subscriber extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      record: {
        topic: "testtopic/react",
        qos: 0,
      },
    };
  }

  onRecordChange = (value) => {
    const { record } = this.state;
    const changedRecord = Object.assign(record, value);
    this.setState({ record: changedRecord });
  };

  handleSubscribe = () => {
    const { topic, qos } = this.state.record;
    this.props.subscribe(topic, qos);
  };

  handleUnsub = () => {
    const { topic } = this.state.record;
    this.props.unsubscribe(topic);
  };

  render() {
    const SubForm = (
      <Form
        layout="vertical"
        name="basic"
        initialValues={this.state.record}
        onValuesChange={this.onRecordChange}
      >
        <Row gutter={20}>
          <Col span={12}>
            <Form.Item label="Topic" name="topic">
              <Input />
            </Form.Item>
          </Col>
          <Col span={12}>
            <QosOption.Consumer>
              {(value) => (
                <Form.Item label="QoS" name="qos">
                  <Select options={value} />
                </Form.Item>
              )}
            </QosOption.Consumer>
          </Col>
          <Col span={8} offset={16} style={{ textAlign: "right" }}>
            <Form.Item>
              <Button
                type="primary"
                htmlType="submit"
                onClick={this.handleSubscribe}
              >
                Subscribe
              </Button>
              {this.props.showUnsub ? (
                <Button
                  type="danger"
                  style={{ marginLeft: "10px" }}
                  onClick={this.handleUnsub}
                >
                  Unsubscribe
                </Button>
              ) : null}
            </Form.Item>
          </Col>
        </Row>
      </Form>
    );

    return <Card title="Subscriber">{SubForm}</Card>;
  }
}

export default Subscriber;
