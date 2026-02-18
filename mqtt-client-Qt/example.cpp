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

// Broker address. QHostAddress supports IP addresses, except for special cases like localhost and null.
// const QHostAddress EXAMPLE_HOST = QHostAddress::Null;
const QHostAddress EXAMPLE_HOST = QHostAddress::LocalHost;
// const QHostAddress EXAMPLE_HOST = QHostAddress("119.23.236.219");
// Broker port.
const quint16 EXAMPLE_PORT = 1883;
// Topic for subscription/publication
const QString EXAMPLE_TOPIC = "qmqtt/example/topic";

class Publisher : public QMQTT::Client
{
    Q_OBJECT
    public:
        // Constructor for publisher client
        explicit Publisher(const QHostAddress& host = EXAMPLE_HOST,
                const quint16 port = EXAMPLE_PORT,
                QObject* parent = NULL)
            : QMQTT::Client(host, port, parent)
              , _number(0)
              , _qout(stdout)
        {
            // Connect signals and slots for connection
            connect(this, &Publisher::connected, this, &Publisher::onConnected);
            // Connect timer and message publishing slot function
            connect(&_timer, &QTimer::timeout, this, &Publisher::onTimeout);
            connect(this, &Publisher::received, this, &Publisher::onReceived);
            connect(this, &Publisher::subscribed, this, &Publisher::onSubscribed);

            // Connect signals and slots for disconnection
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
            // Start or restart the timer with millisecond precision.
            _timer.start(1000);
        }

        // Periodically publish messages, stop after 1000 messages
        void onTimeout()
        {
            QMQTT::Message message(_number, EXAMPLE_TOPIC,
                    QString("Number is %1").arg(_number).toUtf8());
            message.setQos(0);
            std::cout << publish(message) << std::endl;
            _number++;

            if(_number >= 1000)
            {
                // Stop the message publishing timer.
                _timer.stop();
                disconnectFromHost();
                // Call the single shot timer to quit the application.
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
        // Constructor for subscriber client
        explicit Subscriber(const QHostAddress& host = EXAMPLE_HOST,
                const quint16 port = EXAMPLE_PORT,
                QObject* parent = NULL)
            : QMQTT::Client(host, port, parent)
              , _qout(stdout)
        {
            // Connect signals and slots for connection establishment and received messages.
            connect(this, &Subscriber::connected, this, &Subscriber::onConnected);
            connect(this, &Subscriber::subscribed, this, &Subscriber::onSubscribed);
            connect(this, &Subscriber::received, this, &Subscriber::onReceived);
        }
        virtual ~Subscriber() {}

        QTextStream _qout;

    public slots:
        // Handler for successful connection
        void onConnected()
        {
            _qout << "connected" << endl;
            subscribe(EXAMPLE_TOPIC, 0);
        }

        // Callback for successful subscription
        void onSubscribed(const QString& topic)
        {
            _qout << "subscribed " << topic << endl;
        }

        // Callback for received messages
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


    // Define a Qt application object, the entry point for Qt GUI programs
    QCoreApplication app(argc, argv);
    Subscriber subscriber;
    Publisher publisher;

    if (s.contains("sub")) {
       std::cout << "sub" << std::endl;

       // Set Hostname, supporting both IP and domain name.
       // subscriber.setHostName("34.211.84.46");
       // subscriber.setHostName("broker.emqx.io");
       // Set MQTT version, supporting V3_1_1 and V3_1_0.
       subscriber.setVersion(QMQTT::V3_1_1);
       // Set client ID
       subscriber.setClientId("sub_client");
       // Set username
       subscriber.setUsername("username");
       // Set password
       subscriber.setPassword("password");
       // Set whether to auto-reconnect
       subscriber.setAutoReconnect(true);
       // Set reconnection interval
       subscriber.setAutoReconnectInterval(100);
       // Set keep-alive interval
       subscriber.setKeepAlive(100);
       // Set clean session
       subscriber.setCleanSession(true);
       // Set will topic
       subscriber.setWillTopic("will/topic");

       QByteArray ba("Hello");
       // Set will message
       subscriber.setWillMessage(ba);
       // Connect to broker
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

    // The exec() function enters the Qt application's event loop to wait for events.
    return app.exec();
}

#include "example.moc"
