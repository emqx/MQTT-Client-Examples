<?php

require('vendor/autoload.php');

use \PhpMqtt\Client\MqttClient;
use \PhpMqtt\Client\ConnectionSettings;
use \Bluerhinos\phpMQTT;

$server   = 'broker.emqx.io';
// TLS port
$port     = 8883;
$clientId = rand(5, 15);
$username = 'emqx_user';
$password = null;
$clean_session = false;

$connectionSettings  = new ConnectionSettings();
$connectionSettings
  ->setUsername($username)
  ->setPassword(null)
  ->setKeepAliveInterval(60)
  ->setConnectTimeout(3)
  ->setUseTls(true)
  ->setTlsSelfSignedAllowed(true);

$mqtt = new MqttClient($server, $port, $clientId);

$mqtt->connect($connectionSettings, $clean_session);

$mqtt->subscribe('emqx/test', function ($topic, $message) {
    printf("Received message on topic [%s]: %s\n", $topic, $message);
}, 0);

$payload = array(
  'protocol' => 'tls',
  'date' => date('Y-m-d H:i:s'),
  'url' => 'https://github.com/emqx/MQTT-Client-Examples'
);
$mqtt->publish('emqx/test', json_encode($payload), 0);

$mqtt->loop(true);
