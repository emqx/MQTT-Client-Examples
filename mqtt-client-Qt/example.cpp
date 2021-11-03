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

using namespace std;


int fn (int i ) {

        cout<< "dadasdasd" << endl;
    return i*i;
}

static int i = fn(8);
// broker 地址.
const QHostAddress EXAMPLE_HOST = QHostAddress::Null;
// const QHostAddress EXAMPLE_HOST = QHostAddress::LocalHost;
// const QHostAddress EXAMPLE_HOST = QHostAddress("119.23.236.219");
// broker 端口.
const quint16 EXAMPLE_PORT = 1883;
// 订阅/发布的主题
const QString EXAMPLE_TOPIC = "qmqtt/exampletopic";

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
    {
        // 连接信号和槽，连接建立的响应和超时响应
        connect(this, &Publisher::connected, this, &Publisher::onConnected);
        connect(&_timer, &QTimer::timeout, this, &Publisher::onTimeout);
        // connect(this, &Publisher::disconnected, this, &Publisher::onDisconnected);
    }
    virtual ~Publisher() {}

    QTimer _timer;
    quint16 _number;

public slots:
    void onConnected()
    {
        subscribe(EXAMPLE_TOPIC, 0);
        _timer.start(1000);
    }

    // 定时发布消息, 消息发布100条以后关闭
    void onTimeout()
    {
        QMQTT::Message message(_number, EXAMPLE_TOPIC,
                               QString("Number is %1").arg(_number).toUtf8());
        message.setQos(2);
        publish(message);
        _number++;
        if(_number >= 100)
        {
            _timer.stop();
            disconnectFromHost();
            QTimer::singleShot(0, qApp, &QCoreApplication::quit);
        }
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
        // connect(this, &Subscriber::disconnected, this, &Subscriber::onConnected);
    }
    virtual ~Subscriber() {}

    QTextStream _qout;

public slots:
    void onConnected()
    {
        _qout << "connected" << endl;
        subscribe(EXAMPLE_TOPIC, 0);
    }

    void onSubscribed(const QString& topic)
    {
        _qout << "subscribed " << topic << endl;
    }

    void onReceived(const QMQTT::Message& message)
    {
        _qout << "publish received: \"" << QString::fromUtf8(message.payload())
              << "\"" << endl;
    }
};


int main(int argc, char** argv)
{
    cout<< "begin main" << endl;
    QCoreApplication app(argc, argv);
    Subscriber subscriber;
    
    // 设置client id
    // subscriber.setHostName("119.23.236.219");
    subscriber.setHostName("iot-platform.cloud");
    subscriber.setVersion(QMQTT::V3_1_1);
    subscriber.setClientId("sub");
    // 设置用户名
    subscriber.setUsername("alvin");
    // 设置密码
    subscriber.setPassword("HHH0000");
    // 设置是否自动重连
    subscriber.setAutoReconnect(true);
    // 设置重连间隔
    subscriber.setAutoReconnectInterval(100);
    // 设置保活时间间隔
    subscriber.setKeepAlive(100);
    // 设置清空会话
    subscriber.setCleanSession(true);
    // 连接到broker
    subscriber.connectToHost();

    Publisher publisher;
    publisher.setHostName("119.23.236.219");
    // publisher.setHostName("iot-platform.cloud");
    publisher.setClientId("pub");
    publisher.setUsername("alvin");
    publisher.setPassword("HHH0000");
    publisher.setAutoReconnect(true);
    publisher.setAutoReconnectInterval(100);
    publisher.setKeepAlive(100);
    publisher.connectToHost();

    return app.exec();
}

#include "example.moc"
