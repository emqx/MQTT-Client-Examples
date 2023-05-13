using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Packets;
using MQTTnet.Protocol;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;
using System.Text;


namespace MqttClient
{
    public static class Client
    {
        public static async Task TlsClientWithCA(string caFile)
        {
            var mqttFactory = new MqttFactory();

            X509Certificate2 cacert = new X509Certificate2(File.ReadAllBytes(caFile));

            using (var mqttClient = mqttFactory.CreateMqttClient())
            {
                var mqttClientOptions = new MqttClientOptionsBuilder().WithTcpServer("192.168.1.139", 8883)
                    .WithCredentials("emqx", "emqx123")
                    .WithTls(
                        new MqttClientOptionsBuilderTlsParameters()
                        {
                            UseTls = true,
                            SslProtocol = System.Security.Authentication.SslProtocols.Tls12,
                            CertificateValidationHandler = (certContext) =>
                            {
                                X509Chain chain = new X509Chain();
                                chain.ChainPolicy.RevocationMode = X509RevocationMode.NoCheck;
                                chain.ChainPolicy.RevocationFlag = X509RevocationFlag.ExcludeRoot;
                                chain.ChainPolicy.VerificationFlags = X509VerificationFlags.NoFlag;
                                chain.ChainPolicy.VerificationTime = DateTime.Now;
                                chain.ChainPolicy.UrlRetrievalTimeout = new TimeSpan(0, 0, 0);
                                chain.ChainPolicy.CustomTrustStore.Add(cacert);
                                chain.ChainPolicy.TrustMode = X509ChainTrustMode.CustomRootTrust;

                                // convert provided X509Certificate to X509Certificate2
                                var x5092 = new X509Certificate2(certContext.Certificate);

                                return chain.Build(x5092);
                            }
                        })
                    .Build();

                mqttClient.ApplicationMessageReceivedAsync += e =>
                {
                    Console.WriteLine("### RECEIVED APPLICATION MESSAGE ###");
                    Console.WriteLine($"+ Topic = {e.ApplicationMessage.Topic}");
                    Console.WriteLine($"+ Payload = {Encoding.UTF8.GetString(e.ApplicationMessage.PayloadSegment)}");
                    Console.WriteLine($"+ QoS = {e.ApplicationMessage.QualityOfServiceLevel}");
                    Console.WriteLine($"+ Retain = {e.ApplicationMessage.Retain}");
                    Console.WriteLine();
                   
                    return Task.CompletedTask;
                };

                using (var timeout = new CancellationTokenSource(5000))
                {
                    await mqttClient.ConnectAsync(mqttClientOptions, timeout.Token);

                    Console.WriteLine("The MQTT client is connected.");


                }

                var mqttSubscribeOptions = mqttFactory.CreateSubscribeOptionsBuilder()
               .WithTopicFilter(
                   f =>
                   {
                       f.WithTopic("mqttnet/samples/topic/2");
                   })
               .Build();

                await mqttClient.SubscribeAsync(mqttSubscribeOptions, CancellationToken.None);

                Console.WriteLine("MQTT client subscribed to topic.");

                Console.WriteLine("Press enter to exit.");
                Console.ReadLine();
            }
        }

        public static async Task TlsClientWithCert(string caFile, string pfx, string password)
        {
            var mqttFactory = new MqttFactory();

            var cacert = X509Certificate.CreateFromCertFile(caFile);
            cacert = new X509Certificate2(cacert.Export(X509ContentType.SerializedCert));
            var clientCert = new X509Certificate2(pfx, password);
            var newCert = new X509Certificate2(clientCert.Export(X509ContentType.SerializedCert));


            using (var mqttClient = mqttFactory.CreateMqttClient())
            {
                var mqttClientOptions = new MqttClientOptionsBuilder().WithTcpServer("192.168.1.139", 8883)
                    .WithCredentials("emqx", "emqx123")
                    .WithTls(
                        new MqttClientOptionsBuilderTlsParameters()
                        {
                            UseTls = true,
                            SslProtocol = System.Security.Authentication.SslProtocols.Tls12,
                            
                            CertificateValidationHandler = (o) =>
                            {
                                return true;
                            },
                            Certificates = new List<X509Certificate>() {
                            cacert, newCert
                          }
                        })
                    .Build();

                mqttClient.ApplicationMessageReceivedAsync += e =>
                {
                    Console.WriteLine("### RECEIVED APPLICATION MESSAGE ###");
                    Console.WriteLine($"+ Topic = {e.ApplicationMessage.Topic}");
                    Console.WriteLine($"+ Payload = {Encoding.UTF8.GetString(e.ApplicationMessage.PayloadSegment)}");
                    Console.WriteLine($"+ QoS = {e.ApplicationMessage.QualityOfServiceLevel}");
                    Console.WriteLine($"+ Retain = {e.ApplicationMessage.Retain}");
                    Console.WriteLine();
                    return Task.CompletedTask;
                };

                using (var timeout = new CancellationTokenSource(5000))
                {
                    await mqttClient.ConnectAsync(mqttClientOptions, timeout.Token);

                    Console.WriteLine("The MQTT client is connected.");


                }

                var mqttSubscribeOptions = mqttFactory.CreateSubscribeOptionsBuilder()
               .WithTopicFilter(
                   f =>
                   {
                       f.WithTopic("mqttnet/samples/topic/2");
                   })
               .Build();

                await mqttClient.SubscribeAsync(mqttSubscribeOptions, CancellationToken.None);

                Console.WriteLine("MQTT client subscribed to topic.");

                Console.WriteLine("Press enter to exit.");
                Console.ReadLine();
            }
        }



    }
}