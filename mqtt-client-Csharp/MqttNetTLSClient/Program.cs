namespace MqttClient
{
    class Program
    {

        static void Main(string[] args)
        {
            // 自签名证书单向认证
            // MqttClient.Client.TlsClientWithCA("E:/certs/cacert.pem").Wait();

            // 双向认证
            MqttClient.Client.TlsClientWithCert(@"E:/certs/cacert.pem", @"E:/certs/client.pfx","123456").Wait();
        }
    }
}