using System;
using System.Text;
using System.Threading.Tasks;
using MQTTnet;
using MQTTnet.Client;

namespace publish
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

        static async Task Publish(IMqttClient client, string topic)
        {
            int msg_count = 0;
            while (true)
            {
                await Task.Delay(1 * 1000);
                string msg = "messages: " + msg_count.ToString();
                var message = new MqttApplicationMessageBuilder()
                    .WithTopic(topic)
                    .WithPayload(Encoding.UTF8.GetBytes(msg))
                    .Build();
                await client.PublishAsync(message);
                Console.WriteLine("Send `{0}` to topic `{1}`", msg, topic);
                msg_count++;
            }
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
            await Publish(client, topic);
        }
    }
}
