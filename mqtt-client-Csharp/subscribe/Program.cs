using System;
using System.Text;
using System.Threading.Tasks;
using MQTTnet;
using MQTTnet.Client;

namespace subscribe
{
    class Program
    {
        static async Task<IMqttClient> ConnectMQTT(string broker, int port, string clientId, string username, string password)
        {
            var factory = new MqttFactory();
            var client = factory.CreateMqttClient();

            var options = new MqttClientOptionsBuilder()
                .WithTcpServer(broker, port)
                .WithCredentials(username, password)
                .WithClientId(clientId)
                .WithCleanSession()
                .Build();

            var result = await client.ConnectAsync(options);
            if (result.ResultCode == MqttClientConnectResultCode.Success)
            {
                Console.WriteLine("Connected to MQTT Broker");
            }
            else
            {
                Console.WriteLine("Failed to connect");
            }
            return client;
        }

        static async Task Subscribe(IMqttClient client, string topic)
        {
            client.ApplicationMessageReceivedAsync += e =>
            {
                Console.WriteLine($"Received `{Encoding.UTF8.GetString(e.ApplicationMessage.PayloadSegment)}` from `{e.ApplicationMessage.Topic}` topic");
                return Task.CompletedTask;
            };

            await client.SubscribeAsync(topic);

            Console.WriteLine($"Subscribed to topic: {topic}");
            Console.ReadLine(); // Keep the program running
        }

        static async Task Main(string[] args)
        {
            string broker = "broker.emqx.io";
            int port = 1883;
            string topic = "Csharp/mqtt";
            string clientId = Guid.NewGuid().ToString();
            string username = "emqx";
            string password = "public";

            var client = await ConnectMQTT(broker, port, clientId, username, password);
            await Subscribe(client, topic);
        }
    }
}
