using System;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

namespace subscribe
{
    class Program
    {
        static MqttClient ConnectMQTT(string broker, int port, string clientId, string username, string password)
        {
            MqttClient client = new MqttClient(broker, port, false, MqttSslProtocols.None, null, null);
            client.Connect(clientId, username, password);
            if (client.IsConnected)
            {
                Console.WriteLine("Connected to MQTT Broker");
            }
            else
            {
                Console.WriteLine("Failed to connect");
            }
            return client;
        }

        static void Subscribe(MqttClient client, string topic)
        {
            client.MqttMsgPublishReceived += (sender, e) =>
            {
                Console.WriteLine($"Received `{System.Text.Encoding.UTF8.GetString(e.Message)}` from `{e.Topic}` topic");
            };

            client.Subscribe(new string[] { topic }, new byte[] { MqttMsgBase.QOS_LEVEL_AT_LEAST_ONCE });

            Console.WriteLine($"Subscribed to topic: {topic}");
            Console.ReadLine(); // Keep the program running
        }

        static void Main(string[] args)
        {
            string broker = "broker.emqx.io";
            int port = 1883;
            string topic = "Csharp/mqtt";
            string clientId = Guid.NewGuid().ToString();
            string username = "emqx";
            string password = "public";

            MqttClient client = ConnectMQTT(broker, port, clientId, username, password);
            Subscribe(client, topic);
        }
    }
}
