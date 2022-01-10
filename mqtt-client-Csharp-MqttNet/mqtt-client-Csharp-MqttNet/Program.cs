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
        public static async Task Main(string[] args)
        {
            bool useTls = false;
            string topic = "/emqx/topic/msg";
            string caFile = @"broker.emqx.io-ca.crt";
            string broker = "broker.emqx.io";

            var options = useTls
                ? new ManagedMqttClientOptionsBuilder()
                  .WithAutoReconnectDelay(TimeSpan.FromSeconds(5))
                  .WithClientOptions(
                      new MqttClientOptionsBuilder()
                          .WithClientId("EMQX_" + Guid.NewGuid().ToString())
                          .WithTcpServer(broker)
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
                  .Build()
                : new ManagedMqttClientOptionsBuilder()
                  .WithAutoReconnectDelay(TimeSpan.FromSeconds(5))
                  .WithClientOptions(
                      new MqttClientOptionsBuilder()
                          .WithClientId("EMQX_" + Guid.NewGuid().ToString())
                          .WithTcpServer(broker)
                          //.WithCredentials("user", "pass")
                          .WithCleanSession()
                          .Build()
                  )
                  .Build();

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
