<?php

require('vendor/autoload.php');

use \Bluerhinos\phpMQTT;

$server   = 'broker.emqx.io';
$port     = 1883;
$clientId = rand(5, 15);
$username = 'emqx_user';
$password = null;
$clean_session = false;

$c = new phpMQTT($server, $port, $clientId, null);
$conn_result = $c -> connect(false, null, $username, $password);

if ($conn_result) {
    $topics['emqx/test'] = array("qos"=>0, "function"=>"procmsg");
    $c -> subscribe($topics);
}

function procmsg($topic, $msg) {
    echo "recv $topic from $msg";
}

while ($c->proc()) {
}