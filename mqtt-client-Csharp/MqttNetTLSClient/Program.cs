namespace MqttClient
{
    class Program
    {

        static void Main(string[] args)
        {
            // One-way TLS authentication: verify the server certificate against the CA
            MqttClient.Client.TlsClientWithCA("./broker.emqx.io-ca.crt").Wait();

            // Two-way TLS authentication (requires your own client certificate):
            // MqttClient.Client.TlsClientWithCert("./broker.emqx.io-ca.crt", "path/to/client.pfx", "password").Wait();
        }
    }
}
