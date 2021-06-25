using System;
using uPLibrary.Networking.M2Mqtt;

namespace publish
{
    class Program
    {
        static MqttClient ConnectMQTT(string broker, int port, string clientId)
        {
            MqttClient client = new MqttClient(broker, port, false, MqttSslProtocols.None, null, null);
            client.Connect(clientId);
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
        static void Publish(MqttClient client, string topic)
        {
            int msg_count = 0;
            while (true)
            {
                System.Threading.Thread.Sleep(1 * 1000);
                string msg = "messages: " + msg_count.ToString();
                client.Publish(topic, System.Text.Encoding.UTF8.GetBytes(msg));
                Console.WriteLine("Send `{0}` to topic `{1}`", msg, topic);
                msg_count++;
            }
        }
        static void Main(string[] args)
        {
            string broker = "broker.emqx.io";
            int port = 1883;
            string topic = "Csharp/mqtt";
            string clientId = Guid.NewGuid().ToString();
            MqttClient client = ConnectMQTT(broker, port, clientId);
            Publish(client, topic);
        }
    }
}
