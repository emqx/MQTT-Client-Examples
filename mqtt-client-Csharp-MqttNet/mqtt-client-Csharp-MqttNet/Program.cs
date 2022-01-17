using System;
using System.Collections.Generic;
using System.Security.Cryptography.X509Certificates;
using System.Threading.Tasks;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Client.Options;
using MQTTnet.Extensions.ManagedClient;

namespace mqttclientCsharpMqttNet
{
    class MainClass
    {
        private static ManagedMqttClientOptions TcpMqttClientOptions(string url)
        {
            return new ManagedMqttClientOptionsBuilder()
                .WithClientOptions(
                    new MqttClientOptionsBuilder()
                        .WithClientId("EMQX_" + Guid.NewGuid().ToString())
                        .WithTcpServer(url)
                        //.WithCredentials("user", "pass")
                        .WithCleanSession()
                        .Build()
                )
                .Build();
        }

        private static ManagedMqttClientOptions TlsMqttClientOptions(string url, string caFile)
        {
            return new ManagedMqttClientOptionsBuilder()
                .WithClientOptions(
                    new MqttClientOptionsBuilder()
                        .WithClientId("EMQX_" + Guid.NewGuid().ToString())
                        .WithTcpServer(url)
                        //.WithCredentials("user", "pass")
                        .WithCleanSession()
                        .WithTls(
                            new MqttClientOptionsBuilderTlsParameters()
                            {
                                UseTls = true,
                                SslProtocol = System.Security.Authentication.SslProtocols.Tls12,
                                Certificates = new List<X509Certificate>()
                                {
                                    // Download from https://www.emqx.com/en/mqtt/public-mqtt5-broker
                                    X509Certificate.CreateFromCertFile(caFile)
                                }
                            }
                        )
                        .Build()
                )
                .Build();
        }

        private static ManagedMqttClientOptions WsMqttClientOptions(string url)
        {
            return new ManagedMqttClientOptionsBuilder()
                .WithClientOptions(
                    new MqttClientOptionsBuilder()
                        .WithClientId("EMQX_" + Guid.NewGuid().ToString())
                        .WithWebSocketServer(url)
                        //.WithCredentials("user", "pass")
                        .WithCleanSession()
                        .Build()
                )
                .Build();
        }

        private static ManagedMqttClientOptions WssMqttClientOptions(string url, string caFile)
        {
            return new ManagedMqttClientOptionsBuilder()
                .WithClientOptions(
                    new MqttClientOptionsBuilder()
                        .WithClientId("EMQX_" + Guid.NewGuid().ToString())
                        .WithWebSocketServer(url)
                        //.WithCredentials("user", "pass")
                        .WithCleanSession()
                        .WithTls(
                            new MqttClientOptionsBuilderTlsParameters()
                            {
                                UseTls = true,
                                SslProtocol = System.Security.Authentication.SslProtocols.Tls12,
                                Certificates = new List<X509Certificate>()
                                {
                                    // Download from https://www.emqx.com/en/mqtt/public-mqtt5-broker
                                    X509Certificate.CreateFromCertFile(caFile)
                                }
                            }
                        )
                        .Build()
                )
                .Build();
        }

        public static async Task Main(string[] args)
        {
            string topic = "/emqx/topic/msg";
            string connectionType = "tcp"; //tcp, tls, ws, wss
            ManagedMqttClientOptions options = null;

            if (args.Length >= 1)
            {
                connectionType = args[0];
                Console.WriteLine("connection: {0}", connectionType);
            }

            switch (connectionType)
            {
                case "tcp":
                    options = TcpMqttClientOptions("broker.emqx.io");
                    Console.WriteLine("tcp connection");
                    break;
                case "tls":
                    options = TlsMqttClientOptions("broker.emqx.io", @"broker.emqx.io-ca.crt");
                    Console.WriteLine("tls connection");
                    break;
                case "ws":
                    options = WsMqttClientOptions("broker.emqx.io:8083/mqtt");
                    Console.WriteLine("ws connection");
                    break;
                case "wss":
                    options = WssMqttClientOptions(
                        "broker.emqx.io:8084/mqtt",
                        @"broker.emqx.io-ca.crt"
                    );
                    Console.WriteLine("wss connection");
                    break;
                default:
                    Console.WriteLine("Unsupported: {0}" + args[1]);
                    return;
            }

            var mqttClient = new MqttFactory().CreateManagedMqttClient();

            await mqttClient.SubscribeAsync(new MqttTopicFilterBuilder().WithTopic(topic).Build());
            mqttClient.UseApplicationMessageReceivedHandler(
                msg =>
                    Console.Write(
                        "Received: '{0}' from '{1}'\n",
                        System.Text.Encoding.UTF8.GetString(msg.ApplicationMessage.Payload),
                        msg.ApplicationMessage.Topic
                    )
            );

            mqttClient.UseConnectedHandler(
                (arg) => Console.WriteLine("Establish connection " + arg.ConnectResult.ResultCode)
            );

            await mqttClient.StartAsync(options);
            Console.Write("Input message for topic '{0}'\n", topic);
            string payload;
            while ((payload = Console.ReadLine()) != "quit")
            {
                Console.Write("Send '{0}' to topic '{1}'\n", payload, topic);

                await mqttClient.PublishAsync(
                    new MqttApplicationMessageBuilder()
                        .WithTopic("/emqx/topic/msg")
                        .WithPayload(payload)
                        .WithExactlyOnceQoS()
                        .Build()
                );

                Console.Write("Input message for topic '{0}', press 'quit' to exit.\n", topic);
            }

            await mqttClient.StopAsync();
        }
    }
}
