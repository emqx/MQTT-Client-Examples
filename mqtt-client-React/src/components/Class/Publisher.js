import React from "react";
import { Card, Form, Input, Row, Col, Button, Select } from "antd";
import { QosOption } from "./index";

class Publisher extends React.Component {
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

  handlePublish = () => {
    const { record } = this.state;
    this.props.publish(record);
  };

  render() {
    const PublishForm = (
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
          <Col span={24}>
            <Form.Item label="Payload" name="payload">
              <Input.TextArea />
            </Form.Item>
          </Col>
          <Col span={8} offset={16} style={{ textAlign: "right" }}>
            <Form.Item>
              <Button
                type="primary"
                htmlType="submit"
                onClick={this.handlePublish}
              >
                Publish
              </Button>
            </Form.Item>
          </Col>
        </Row>
      </Form>
    );

    return <Card title="Publisher">{PublishForm}</Card>;
  }
}

export default Publisher;
