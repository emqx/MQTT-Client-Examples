# sub.py
import time
from umqtt.simple import MQTTClient

# 定义 sub 客户端的连接信息
SERVER="broker.emqx.io"
ClientID = f'raspberry-sub-{time.time_ns()}'
user = "emqx"
password = "public"
topic = "raspberry/mqtt"
msg = b'{"msg":"hello"}'

def sub(topic, msg):
    # 在回调函数打印主题和消息
    print('received message %s on topic %s' % (msg, topic))

def main(server=SERVER):
    # 创建连接，参数分别为客户端 ID，broker 地址，broker 端口号，认证信息
    client = MQTTClient(ClientID, server, 1883, user, password)
    client.set_callback(sub)
    client.connect()
    print('Connected to MQTT Broker "%s"' % (server))
    # 如果与 broker 失去连接后重连，仍然会继续订阅 raspberry/topic 主题
    client.subscribe(topic)
    while True:
        if True:
            client.wait_msg()
        else:
            client.check_msg()
            time.sleep(1)

if __name__ == "__main__":
    main()
