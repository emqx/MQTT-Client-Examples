# PHP MQTT 客户端使用示例

在本示例中我们提供了 TCP、TLS 协议连接到 MQTT Broker 示例代码。

示例选择了 composer 上下载量最高的 `php-mqtt/client` 这个客户端库，也有[一些](https://packagist.org/search/?query=mqtt)优秀的基于 [Swoole](https://www.swoole.com/) 的客户端库欢迎大家去使用，提交 sample 与名单到此仓库中。

有关 php-mqtt/client 更多使用文档请参阅 [Packagist php-mqtt/client](https://packagist.org/packages/php-mqtt/client)。

## 前提

PHP 7.4 及以上版本

## 安装

```bash
composer require php-mqtt/client
```

## 运行

```bash
python pubsub_tcp.php
```

## 参阅

[如何在 PHP 中使用 MQTT](https://www.emqx.com/zh/blog/how-to-use-mqtt-in-php)
