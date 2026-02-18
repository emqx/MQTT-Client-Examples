namespace MqttClient
{
    class Program
    {

        static void Main(string[] args)
        {
            // Self-signed certificate one-way authentication
            // MqttClient.Client.TlsClientWithCA("E:/certs/cacert.pem").Wait();

            // Two-way authentication
            MqttClient.Client.TlsClientWithCert(@"E:/certs/cacert.pem", @"E:/certs/client.pfx","123456").Wait();
        }
    }
}