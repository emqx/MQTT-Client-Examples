# pub.py
import time
from umqtt.simple import MQTTClient

# 定义 pub 客户端的连接信息
server="broker.emqx.io"
ClientID = f'raspberry-pub-{time.time_ns()}'
user = "emqx"
password = "public"
topic = "raspberry/mqtt"
msg = b'{"msg":"hello"}'

# 创建连接，参数分别为客户端 ID，broker 地址，broker 端口号，认证信息
def connect():
    print('Connected to MQTT Broker "%s"' % (server))
    client = MQTTClient(ClientID, server, 1883, user, password)
    client.connect()
    return client

def reconnect():
    # 若无法连接到 broker，打印一条消息以通知连接不成功，并且等待 5 秒发起重连
    print('Failed to connect to MQTT broker, Reconnecting...' % (server))
    time.sleep(5)
    client.reconnect()

# 若能连接到 broker，调用 connect()，反之调用 reconnect()
try:
    client = connect()
except OSError as e:
    reconnect()

# 每隔 1 秒给主题 raspberry/mqtt 发送一条消息
while True:
  print('send message %s on topic %s' % (msg, topic))
  client.publish(topic, msg, qos=0)
  time.sleep(1)
