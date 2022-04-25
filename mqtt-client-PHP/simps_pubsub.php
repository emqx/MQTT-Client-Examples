<?php
require('vendor/autoload.php');

use Simps\MQTT\Client;
use Simps\MQTT\Config\ClientConfig;
use Swoole\Coroutine;
use Simps\MQTT\Protocol\Types;

function getConfig()
{
    $config = new ClientConfig();
    $config->setUserName('emqx_user')
    ->setPassword('public')
    ->setClientId('emqx_example_'.Client::genClientID())
    ->setKeepAlive(10)
    // 3,4,5
    ->setProtocolLevel(5);
    return $config;
}


// Subscribe
// https://github.com/simps/mqtt/blob/master/examples/v5/subscribe.php
Coroutine\run(function() {
  $config = getConfig();
  $client = new Client('broker.emqx.io', 1883, $config);
  $client->connect();
  $client->subscribe(['emqx/test' => 0]);
  while(true) {
    $buffer = $client->recv();
    var_dump($buffer);
  };
});

// Publish
// https://github.com/simps/mqtt/blob/master/examples/v5/publish.php
Coroutine\run(function () {
    // refer: https://github.com/simps/mqtt/blob/master/examples/bootstrap.php
    $config = getConfig();

    $client = new Client('broker.emqx.io', 1883, $config);
    $client->connect();
    $payload = array(
    'protocol' => 'MQTT 5.0',
    'date' => date('Y-m-d H:i:s'),
    'url' => 'https://github.com/emqx/MQTT-Client-Examples');
    while (true) {
        $response = $client->publish(
            'emqx/test',
            json_encode(
              $payload
          ),
            1,
            0,
            0,
            [
              'topic_alias' => 1,
              'message_expiry_interval' => 12,
          ]
        );
        $msgid = $response['message_id'];
        printf("msg $msgid send\n");
        Coroutine::sleep(3);
    }
});
