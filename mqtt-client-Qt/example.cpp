/*
 * example.cpp - qmqtt example
 *
 * Copyright (c) 2013  Ery Lee <ery.lee at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of mqttc nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <qmqtt.h>
#include <QCoreApplication>
#include <QTimer>
#include <iostream>

// broker 地址, QHostAddress 除了localhost 和 null 等特殊情况，支持 ip 方式。
// const QHostAddress EXAMPLE_HOST = QHostAddress::Null;
const QHostAddress EXAMPLE_HOST = QHostAddress::LocalHost;
// const QHostAddress EXAMPLE_HOST = QHostAddress("119.23.236.219");
// broker 端口.
const quint16 EXAMPLE_PORT = 1883;
// 订阅/发布的主题
const QString EXAMPLE_TOPIC = "qmqtt/example/topic";

class Publisher : public QMQTT::Client
{
    Q_OBJECT
    public:
        // 构造发布客户端
        explicit Publisher(const QHostAddress& host = EXAMPLE_HOST,
                const quint16 port = EXAMPLE_PORT,
                QObject* parent = NULL)
            : QMQTT::Client(host, port, parent)
              , _number(0)
              , _qout(stdout)
        {
            // 关联连接的信号和槽函数
            connect(this, &Publisher::connected, this, &Publisher::onConnected);
            // 关联定时器和消息发布槽函数
            connect(&_timer, &QTimer::timeout, this, &Publisher::onTimeout);
            connect(this, &Publisher::received, this, &Publisher::onReceived);
            connect(this, &Publisher::subscribed, this, &Publisher::onSubscribed);

            // 关联断开的信号和槽函数
            connect(this, &Publisher::disconnected, this, &Publisher::onDisconnected);
        }
        virtual ~Publisher() {}

        QTimer _timer;
        quint16 _number;
        QTextStream _qout;

    public slots:
        void onConnected()
        {
            subscribe("will/topic", 0);
            // 启动或重启定时器，毫秒精度。
            _timer.start(1000);
        }

        // 定时发布消息, 消息发布100条以后关闭
        void onTimeout()
        {
            QMQTT::Message message(_number, EXAMPLE_TOPIC,
                    QString("Number is %1").arg(_number).toUtf8());
            message.setQos(0);
            std::cout << publish(message) << std::endl;
            _number++;

            if(_number >= 1000)
            {
                // 停止发布消息定时器。
                _timer.stop();
                disconnectFromHost();
                // 调用单次定时器关闭槽函数。
                QTimer::singleShot(0, qApp, &QCoreApplication::quit);
            }
        }


        void onSubscribed(const QString& topic)
        {
            _qout << "subscribed " << topic << endl;
        }


        void onReceived(const QMQTT::Message& message)
        {
            _qout << "Received from topic: \"" << message.topic()
                << "\"" << endl;
            _qout << "Received payload: \"" << QString::fromUtf8(message.payload())
                << "\"" << endl;
        }

        void onDisconnected()
        {
            QTimer::singleShot(0, qApp, &QCoreApplication::quit);
        }
};

class Subscriber : public QMQTT::Client
{
    Q_OBJECT
    public:
        // 构造订阅客户端
        explicit Subscriber(const QHostAddress& host = EXAMPLE_HOST,
                const quint16 port = EXAMPLE_PORT,
                QObject* parent = NULL)
            : QMQTT::Client(host, port, parent)
              , _qout(stdout)
        {
            // 连接信号和槽，连接建立、收到订阅的主题及消息的相应。
            connect(this, &Subscriber::connected, this, &Subscriber::onConnected);
            connect(this, &Subscriber::subscribed, this, &Subscriber::onSubscribed);
            connect(this, &Subscriber::received, this, &Subscriber::onReceived);
        }
        virtual ~Subscriber() {}

        QTextStream _qout;

    public slots:
        // 连接成功是处理函数
        void onConnected()
        {
            _qout << "connected" << endl;
            subscribe(EXAMPLE_TOPIC, 0);
        }

        // 订阅成功的回调
        void onSubscribed(const QString& topic)
        {
            _qout << "subscribed " << topic << endl;
        }

        // 收到消息的回调
        void onReceived(const QMQTT::Message& message)
        {
            _qout << "Received from topic: \"" << message.topic()
                << "\"" << endl;
            _qout << "Received payload: \"" << QString::fromUtf8(message.payload())
                << "\"" << endl;
        }
};


int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Usage: qmqtt_example [pub | sub]" << std::endl;
        return -1;
    }

    QString s = QString(argv[1]);
    if (!s.contains("sub") && !s.contains("pub")) {

        QTextStream qout(stdout);
        qout<< "Unknown arguments: " << s << endl;
        std::cout << "Usage: qmqtt_example [pub | sub]" << std::endl;
        return -1;
    }


    // 定义一个 Qt 应用程序对象, Qt 图形界面程序的入口
    QCoreApplication app(argc, argv);
    Subscriber subscriber;
    Publisher publisher;

    if (s.contains("sub")) {
       std::cout << "sub" << std::endl;

       // 设置 Hostname， 这里支持ip和域名两种方式。
       // subscriber.setHostName("34.211.84.46");
       // subscriber.setHostName("broker.emqx.io");
       // 设置 mqtt 版本, 支持 V3_1_1 和 V3_1_0。
       subscriber.setVersion(QMQTT::V3_1_1);
       // 设置 client id
       subscriber.setClientId("sub_client");
       // 设置用户名
       subscriber.setUsername("username");
       // 设置密码
       subscriber.setPassword("password");
       // 设置是否自动重连
       subscriber.setAutoReconnect(true);
       // 设置重连间隔
       subscriber.setAutoReconnectInterval(100);
       // 设置保活时间间隔
       subscriber.setKeepAlive(100);
       // 设置清空会话
       subscriber.setCleanSession(true);
       // 设置遗愿主题
       subscriber.setWillTopic("will/topic");

       QByteArray ba("Hello");
       // 设置遗愿消息
       subscriber.setWillMessage(ba);
       // 连接到broker
       subscriber.connectToHost();
    } else if (s.contains("pub")) {

        // publisher.setHostName("broker.emqx.io");
        publisher.setClientId("pub_client11");
        publisher.setUsername("username");
        publisher.setPassword("password");
        publisher.setAutoReconnect(true);
        publisher.setAutoReconnectInterval(100);
        publisher.setKeepAlive(100);
        publisher.connectToHost();
    }

    // exec() 函数会进入 Qt 应用程序的事件循环函数等待事件触发。
    return app.exec();
}

#include "example.moc"
