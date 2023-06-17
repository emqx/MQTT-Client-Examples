package info.mqtt.android.extsample.model

import info.mqtt.android.extsample.internal.Connection
import info.mqtt.android.service.QoS

class ConnectionModel {
    var clientHandle: String = ""
    var clientId: String = "ExtendedSampleClient"
    var serverHostName: String = "broker.hivemq.com"
    var serverPort = 1883
    var isCleanSession = true
    var username: String = ""
    var password: String = ""
    var isTlsConnection = false
    var tlsServerKey: String = ""
    var tlsClientKey: String = ""
    var timeout = 80
    var keepAlive = 200
    var lwtTopic: String = ""
    var lwtMessage: String = ""
    var lwtQos = QoS.AtMostOnce
    var isLwtRetain = false

    constructor()

    constructor(connection: Connection) {
        clientHandle = connection.handle()
        clientId = connection.id
        serverHostName = connection.hostName
        serverPort = connection.port
        isCleanSession = connection.connectionOptions.isCleanSession
        username = if (connection.connectionOptions.userName == null) {
            ""
        } else {
            connection.connectionOptions.userName
        }
        password = if (connection.connectionOptions.password != null) {
            String(connection.connectionOptions.password)
        } else {
            ""
        }
        tlsServerKey = "--- TODO ---"
        tlsClientKey = "--- TODO ---"
        timeout = connection.connectionOptions.connectionTimeout
        keepAlive = connection.connectionOptions.keepAliveInterval
        lwtTopic = if (connection.connectionOptions.willDestination == null) {
            ""
        } else {
            connection.connectionOptions.willDestination
        }
        if (connection.connectionOptions.willMessage != null) {
            lwtMessage = String(connection.connectionOptions.willMessage.payload)
            lwtQos = QoS.valueOf(connection.connectionOptions.willMessage.qos)
            isLwtRetain = connection.connectionOptions.willMessage.isRetained
        } else {
            lwtMessage = ""
            lwtQos = QoS.AtMostOnce
            isLwtRetain = false
        }
    }

    override fun toString(): String {
        return "ConnectionModel{" +
                "clientHandle='" + clientHandle + '\'' +
                ", clientId='" + clientId + '\'' +
                ", serverHostName='" + serverHostName + '\'' +
                ", serverPort=" + serverPort +
                ", cleanSession=" + isCleanSession +
                ", username='" + username + '\'' +
                ", password='" + password + '\'' +
                ", tlsConnection=" + isTlsConnection +
                ", tlsServerKey='" + tlsServerKey + '\'' +
                ", tlsClientKey='" + tlsClientKey + '\'' +
                ", timeout=" + timeout +
                ", keepAlive=" + keepAlive +
                ", lwtTopic='" + lwtTopic + '\'' +
                ", lwtMessage='" + lwtMessage + '\'' +
                ", lwtQos=" + lwtQos +
                ", lwtRetain=" + isLwtRetain +
                '}'
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) {
            return true
        }
        if (other == null || javaClass != other.javaClass) {
            return false
        }
        val that = other as ConnectionModel
        if (serverPort != that.serverPort) {
            return false
        }
        if (isCleanSession != that.isCleanSession) {
            return false
        }
        if (isTlsConnection != that.isTlsConnection) {
            return false
        }
        if (timeout != that.timeout) {
            return false
        }
        if (keepAlive != that.keepAlive) {
            return false
        }
        if (lwtQos != that.lwtQos) {
            return false
        }
        if (isLwtRetain != that.isLwtRetain) {
            return false
        }
        if (clientHandle != that.clientHandle) {
            return false
        }
        if (clientId != that.clientId) {
            return false
        }
        if (serverHostName != that.serverHostName) {
            return false
        }
        if (username != that.username) {
            return false
        }
        if (password != that.password) {
            return false
        }
        if (tlsServerKey != that.tlsServerKey) {
            return false
        }
        if (tlsClientKey != that.tlsClientKey) {
            return false
        }
        return if (lwtTopic != that.lwtTopic) {
            false
        } else lwtMessage == that.lwtMessage
    }

    override fun hashCode(): Int {
        var result = clientHandle.hashCode()
        result = 31 * result + clientId.hashCode()
        result = 31 * result + serverHostName.hashCode()
        result = 31 * result + serverPort
        result = 31 * result + if (isCleanSession) 1 else 0
        result = 31 * result + username.hashCode()
        result = 31 * result + password.hashCode()
        result = 31 * result + if (isTlsConnection) 1 else 0
        result = 31 * result + tlsServerKey.hashCode()
        result = 31 * result + tlsClientKey.hashCode()
        result = 31 * result + timeout
        result = 31 * result + keepAlive
        result = 31 * result + lwtTopic.hashCode()
        result = 31 * result + lwtMessage.hashCode()
        result = 31 * result + lwtQos.value
        result = 31 * result + if (isLwtRetain) 1 else 0
        return result
    }
}
