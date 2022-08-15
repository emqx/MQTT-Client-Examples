package io.emqx.mqtt

class Publish(var topic: String, var payload: String, var qos: Int, var isRetained: Boolean)