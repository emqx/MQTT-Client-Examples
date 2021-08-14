#include <string.h>
#include <stdint.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <dfs_posix.h>
#include <sys/time.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "MQTTPacket.h"
#include "paho_mqtt.h"

#define DBG_ENABLE
#define DBG_SECTION_NAME    "mqtt"
#ifdef MQTT_DEBUG
#define DBG_LEVEL           DBG_LOG
#else
#define DBG_LEVEL           DBG_INFO
#endif /* MQTT_DEBUG */
#define DBG_COLOR
#include <rtdbg.h>

#ifndef LOG_D
#error "Please update the 'rtdbg.h' file to GitHub latest version (https://github.com/RT-Thread/rt-thread/blob/master/include/rtdbg.h)"
#endif

#ifndef RT_PKG_MQTT_THREAD_STACK_SIZE
#ifdef MQTT_USING_TLS
#define RT_PKG_MQTT_THREAD_STACK_SIZE 6144
#else
#define RT_PKG_MQTT_THREAD_STACK_SIZE 4096
#endif
#endif

#ifdef MQTT_USING_TLS
#if (RT_PKG_MQTT_THREAD_STACK_SIZE < 6144)
#error "MQTT using tls, please increase MQTT thread stack size up to 6K via menuconfig tool!"
#endif
#endif

/*
 * resolve server address
 * @param server the server sockaddress
 * @param url the input URL address.
 * @param host_addr the buffer pointer to save server host address
 * @param request the pointer to point the request url, for example, /index.html
 *
 * @return 0 on resolve server address OK, others failed
 *
 * URL example:
 * tcp://192.168.10.151:1883
 * tls://192.168.10.151:61614
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * tls://[fe80::20c:29ff:fe9a:a07e]:61614
 */
static int mqtt_resolve_uri(MQTTClient *c, struct addrinfo **res)
{
    int rc = 0;
    int uri_len = 0, host_addr_len = 0, port_len = 0;
    char *ptr;
    char port_str[6] = {0};      /* default port of mqtt(http) */

    const char *host_addr = 0;
    char *host_addr_new = RT_NULL;
    const char *uri = c->uri;
    uri_len = strlen(uri);

    /* strip protocol(tcp or ssl) */
    if (strncmp(uri, "tcp://", 6) == 0)
    {
        host_addr = uri + 6;
    }
    else if (strncmp(uri, "ssl://", 6) == 0)
    {
        host_addr = uri + 6;

#ifndef MQTT_USING_TLS
        LOG_W("Warning: tls uri, please enable mqtt tls support!");
        rc = -1;
        goto _exit;
#endif
    }
    else
    {
        rc = -1;
        goto _exit;
    }

    /* ipv6 address */
    if (host_addr[0] == '[')
    {
        host_addr += 1;
        ptr = strstr(host_addr, "]");
        if (!ptr)
        {
            rc = -1;
            goto _exit;
        }
        host_addr_len = ptr - host_addr;
        if ((host_addr_len < 1) || (host_addr_len > uri_len))
        {
            rc = -1;
            goto _exit;
        }

        port_len = uri_len - 6 - host_addr_len - 3;
        if (port_len >= 6 || port_len < 1)
        {
            rc = -1;
            goto _exit;
        }

        strncpy(port_str, host_addr + host_addr_len + 2, port_len);
        port_str[port_len] = '\0';
        LOG_D("ipv6 address port: %s", port_str);
    }
    else /* ipv4 or domain. */
    {
        ptr = strstr(host_addr, ":");
        if (!ptr)
        {
            rc = -1;
            goto _exit;
        }
        host_addr_len = ptr - host_addr;
        if ((host_addr_len < 1) || (host_addr_len > uri_len))
        {
            rc = -1;
            goto _exit;
        }

        port_len = uri_len - 6 - host_addr_len - 1;
        if (port_len >= 6 || port_len < 1)
        {
            rc = -1;
            goto _exit;
        }

        strncpy(port_str, host_addr + host_addr_len + 1, port_len);
        port_str[port_len] = '\0';
        LOG_D("ipv4 address port: %s", port_str);
    }

    /* get host addr ok. */
    {
        /* resolve the host name. */
        struct addrinfo hint;
        int ret;

        host_addr_new = rt_malloc(host_addr_len + 1);

        if (!host_addr_new)
        {
            rc = -1;
            goto _exit;
        }

        memcpy(host_addr_new, host_addr, host_addr_len);
        host_addr_new[host_addr_len] = '\0';
        LOG_D("HOST = '%s'", host_addr_new);

#ifdef MQTT_USING_TLS
        if (c->tls_session)
        {
            c->tls_session->host = rt_strdup(host_addr_new);
            c->tls_session->port = rt_strdup(port_str);
        }
#endif

        memset(&hint, 0, sizeof(hint));

        ret = getaddrinfo(host_addr_new, port_str, &hint, res);
        if (ret != 0)
        {
            LOG_E("getaddrinfo err: %d '%s'", ret, host_addr_new);
            rc = -1;
            goto _exit;
        }
    }

_exit:
    if (host_addr_new != RT_NULL)
    {
        rt_free(host_addr_new);
        host_addr_new = RT_NULL;
    }
    return rc;
}

#ifdef MQTT_USING_TLS
static int mqtt_open_tls(MQTTClient *c)
{
    int tls_ret = 0;
    const char *pers = "mqtt";

    if (!c)
        return -RT_ERROR;

    c->tls_session = (MbedTLSSession *)rt_malloc(sizeof(MbedTLSSession));
    if (c->tls_session == RT_NULL)
    {
        LOG_E("open tls failed, no memory for tls_session buffer malloc");
        return -RT_ENOMEM;
    }
    memset(c->tls_session, 0x0, sizeof(MbedTLSSession));

    c->tls_session->buffer_len = MQTT_TLS_READ_BUFFER;
    c->tls_session->buffer = rt_malloc(c->tls_session->buffer_len);
    if (c->tls_session->buffer == RT_NULL)
    {
        LOG_E("open tls failed, no memory for tls_session buffer malloc");
        rt_free(c->tls_session);
        c->tls_session = RT_NULL;
        return -RT_ENOMEM;
    }

    if ((tls_ret = mbedtls_client_init(c->tls_session, (void *)pers, strlen(pers))) < 0)
    {
        LOG_E("mbedtls_client_init err return : -0x%x", -tls_ret);
        return -RT_ERROR;
    }

    return RT_EOK;
}
#endif

static int net_connect(MQTTClient *c)
{
    int rc = -1;
    struct addrinfo *addr_res = RT_NULL;

    c->sock = -1;
    c->next_packetid = 0;

#ifdef MQTT_USING_TLS
    if (strncmp(c->uri, "ssl://", 6) == 0)
    {
        if (mqtt_open_tls(c) < 0)
        {
            LOG_E("mqtt_open_tls err!");
            return -RT_ERROR;
        }
    }
    else
    {
        c->tls_session = RT_NULL;
    }
#endif

    rc = mqtt_resolve_uri(c, &addr_res);
    if (rc < 0 || addr_res == RT_NULL)
    {
        LOG_E("resolve uri err");
        goto _exit;
    }

#ifdef MQTT_USING_TLS
    if (c->tls_session)
    {
        int tls_ret = 0;
        int timeout = MQTT_SOCKET_TIMEO;

        if ((tls_ret = mbedtls_client_context(c->tls_session)) < 0)
        {
            LOG_E("mbedtls_client_context err return : -0x%x", -tls_ret);
            return -RT_ERROR;
        }

        if ((tls_ret = mbedtls_client_connect(c->tls_session)) < 0)
        {
            LOG_E("mbedtls_client_connect err return : -0x%x", -tls_ret);
            rc = -RT_ERROR;
            goto _exit;
        }
        LOG_D("tls connect success...");

        c->sock = c->tls_session->server_fd.fd;

        /* set recv timeout option */
        setsockopt(c->sock, SOL_SOCKET, SO_RCVTIMEO, (void *) &timeout,
                   sizeof(timeout));
        setsockopt(c->sock, SOL_SOCKET, SO_SNDTIMEO, (void *) &timeout,
                   sizeof(timeout));

        rc = 0;
        goto _exit;
    }
#endif

    if ((c->sock = socket(addr_res->ai_family, SOCK_STREAM, 0)) == -1)
    {
        LOG_E("create socket error!");
        goto _exit;
    }

    if ((rc = connect(c->sock, addr_res->ai_addr, addr_res->ai_addrlen)) == -1)
    {
        LOG_E("connect err!");
        closesocket(c->sock);
        c->sock = -1;

        rc = -2;
        goto _exit;
    }

_exit:
    if (addr_res)
    {
        freeaddrinfo(addr_res);
        addr_res = RT_NULL;
    }
    return rc;
}

static int net_disconnect(MQTTClient *c)
{
#ifdef MQTT_USING_TLS
    if (c->tls_session)
    {
        mbedtls_client_close(c->tls_session);
        c->sock = -1;
        return 0;
    }
#endif

    if (c->sock >= 0)
    {
        closesocket(c->sock);
        c->sock = -1;
    }

    return 0;
}

static int net_disconnect_exit(MQTTClient *c)
{
    int i;

    net_disconnect(c);

    if (c->buf && c->readbuf)
    {
        rt_free(c->buf);
        rt_free(c->readbuf);
    }

    if (c->pub_mutex)
    {
        rt_mutex_delete(c->pub_mutex);
    }

    if (c->pipe_device)
    {
        close(c->pub_pipe[0]);
        close(c->pub_pipe[1]);
        rt_pipe_delete((const char *)c->pipe_device->parent.parent.name);
    }

    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter)
        {
            rt_free(c->messageHandlers[i].topicFilter);
            c->messageHandlers[i].topicFilter = RT_NULL;
            c->messageHandlers[i].callback = RT_NULL;
        }
    }
    
    c->isconnected = 0;

    return 0;
}

static int sendPacket(MQTTClient *c, int length)
{
    int rc;
    struct timeval tv;

    tv.tv_sec = 2000;
    tv.tv_usec = 0;

    setsockopt(c->sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));

#ifdef MQTT_USING_TLS
    if (c->tls_session)
    {
        rc = mbedtls_client_write(c->tls_session, c->buf, length);
        goto _continue;
    }
#endif

    rc = send(c->sock, c->buf, length, 0);

#ifdef MQTT_USING_TLS
_continue:
#endif
    if (rc == length)
    {
        rc = 0;
    }
    else
    {
        rc = -1;
    }

    return rc;
}

static int net_read(MQTTClient *c, unsigned char *buf,  int len, int timeout)
{
    int bytes = 0;
    int rc;

    while (bytes < len)
    {

#ifdef MQTT_USING_TLS
        if (c->tls_session)
        {
            rc = mbedtls_client_read(c->tls_session, &buf[bytes], (size_t)(len - bytes));
            if (rc <= 0)
            {
                bytes = -1;
                break;
            }
            else
            {
                bytes += rc;
            }
            goto _continue;
        }
#endif

        rc = recv(c->sock, &buf[bytes], (size_t)(len - bytes), MSG_DONTWAIT);

        if (rc == -1)
        {
            if (errno != ENOTCONN && errno != ECONNRESET)
            {
                bytes = -1;
                break;
            }
        }
        else
            bytes += rc;

#ifdef MQTT_USING_TLS
_continue:
#endif
        if (bytes >= len)
        {
            break;
        }

        if (timeout > 0)
        {
            fd_set readset;
            struct timeval interval;

            LOG_D("net_read %d:%d, timeout:%d", bytes, len, timeout);
            timeout  = 0;

            interval.tv_sec = 1;
            interval.tv_usec = 0;

            FD_ZERO(&readset);
            FD_SET(c->sock, &readset);

            select(c->sock + 1, &readset, RT_NULL, RT_NULL, &interval);
        }
        else
        {
            LOG_D("net_read %d:%d, break!", bytes, len);
            break;
        }
    }

    return bytes;
}

static int decodePacket(MQTTClient *c, int *value, int timeout)
{
    unsigned char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    *value = 0;
    do
    {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
        {
            rc = MQTTPACKET_READ_ERROR; /* bad data */
            goto exit;
        }
        rc = net_read(c, &i, 1, timeout);
        if (rc != 1)
            goto exit;
        *value += (i & 127) * multiplier;
        multiplier *= 128;
    }
    while ((i & 128) != 0);
exit:
    return len;
}

static int MQTTPacket_readPacket(MQTTClient *c)
{
    int rc = PAHO_FAILURE;
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;

    /* 1. read the header byte.  This has the packet type in it */
    if (net_read(c, c->readbuf, 1, 0) != 1)
        goto exit;

    len = 1;
    /* 2. read the remaining length.  This is variable in itself */
    decodePacket(c, &rem_len, 50);
    len += MQTTPacket_encode(c->readbuf + 1, rem_len); /* put the original remaining length back into the buffer */

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if (rem_len > 0 && (net_read(c, c->readbuf + len, rem_len, 300) != rem_len))
        goto exit;

    header.byte = c->readbuf[0];
    rc = header.bits.type;

exit:
    return rc;
}

static int getNextPacketId(MQTTClient *c)
{
    return c->next_packetid = (c->next_packetid == MAX_PACKET_ID) ? 1 : c->next_packetid + 1;
}

static int MQTTConnect(MQTTClient *c)
{
    int rc = -1, len;
    MQTTPacket_connectData *options = &c->condata;

    if (c->isconnected) /* don't send connect packet again if we are already connected */
        goto _exit;

    c->keepAliveInterval = options->keepAliveInterval;

    if ((len = MQTTSerialize_connect(c->buf, c->buf_size, options)) <= 0)
        goto _exit;

    if ((rc = sendPacket(c, len)) != 0)  // send the connect packet
        goto _exit; // there was a problem

    {
        int res;
        fd_set readset;
        struct timeval timeout;

        timeout.tv_sec = c->connect_timeout ? c->connect_timeout / RT_TICK_PER_SECOND : 5;
        timeout.tv_usec = c->connect_timeout ? (c->connect_timeout % RT_TICK_PER_SECOND) * 1000 : 0;

        FD_ZERO(&readset);
        FD_SET(c->sock, &readset);

        res = select(c->sock + 1, &readset, RT_NULL, RT_NULL, &timeout);

        if (res <= 0)
        {
            LOG_E("%s wait resp fail, res:%d errno:%d", __FUNCTION__, res, errno);
            rc = -1;
            goto _exit;
        }
    }

    rc = MQTTPacket_readPacket(c);
    if (rc < 0)
    {
        LOG_E("%s MQTTPacket_readPacket fail", __FUNCTION__);
        goto _exit;
    }

    if (rc == CONNACK)
    {
        unsigned char sessionPresent, connack_rc;

        if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, c->readbuf, c->readbuf_size) == 1)
        {
            rc = connack_rc;
        }
        else
        {
            rc = -1;
        }
    }
    else
        rc = -1;

_exit:
    if (rc == 0)
        c->isconnected = 1;

    return rc;
}

static int MQTTDisconnect(MQTTClient *c)
{
    int rc = PAHO_FAILURE;
    int len = 0;

    len = MQTTSerialize_disconnect(c->buf, c->buf_size);
    if (len > 0)
        rc = sendPacket(c, len);            // send the disconnect packet

    c->isconnected = 0;

    return rc;
}

/**
 * This function subscribe specified mqtt topic.
 *
 * @param c the pointer of MQTT context structure
 * @param topicFilter topic filter name
 * @param qos requested QoS
 *
 * @return the error code, 0 on subscribe successfully.
 */
static int MQTTSubscribe(MQTTClient *c, const char *topicFilter, enum QoS qos)
{
    int rc = PAHO_FAILURE;
    int len = 0;
    int qos_sub = qos;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicFilter;

    if (!c->isconnected)
        goto _exit;

    len = MQTTSerialize_subscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic, &qos_sub);
    if (len <= 0)
        goto _exit;
    if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the subscribe packet
        goto _exit;             // there was a problem

    {
        int res;
        fd_set readset;
        struct timeval timeout;

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        FD_ZERO(&readset);
        FD_SET(c->sock, &readset);

        res = select(c->sock + 1, &readset, RT_NULL, RT_NULL, &timeout);

        if (res <= 0)
        {
            LOG_E("%s wait resp fail, res:%d errno:%d", __FUNCTION__, res, errno);
            rc = -1;
            goto _exit;
        }
    }

    rc = MQTTPacket_readPacket(c);
    if (rc < 0)
    {
        LOG_E("MQTTSubscribe MQTTPacket_readPacket MQTTConnect fail");
        goto _exit;
    }

    if (rc == SUBACK)      // wait for suback
    {
        int count = 0, grantedQoS = -1;
        unsigned short mypacketid;

        if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, c->readbuf, c->readbuf_size) == 1)
            rc = grantedQoS; // 0, 1, 2 or 0x80

        if (rc != 0x80)
        {
            rc = 0;
        }
    }
    else
        rc = PAHO_FAILURE;

_exit:
    return rc;
}

static void NewMessageData(MessageData *md, MQTTString *aTopicName, MQTTMessage *aMessage)
{
    md->topicName = aTopicName;
    md->message = aMessage;
}

// assume topic filter and name is in correct format
// # can only be at end
// + and # can only be next to separator
static char isTopicMatched(char *topicFilter, MQTTString *topicName)
{
    char *curf = topicFilter;
    char *curn = topicName->lenstring.data;
    char *curn_end = curn + topicName->lenstring.len;

    while (*curf && curn < curn_end)
    {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+')
        {
            // skip until we meet the next separator, or end of string
            char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1;    // skip until end of string
        curf++;
        curn++;
    };

    return (curn == curn_end) && (*curf == '\0');
}

static int deliverMessage(MQTTClient *c, MQTTString *topicName, MQTTMessage *message)
{
    int i;
    int rc = PAHO_FAILURE;

    // we have to find the right message handler - indexed by topic
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter != 0 && (MQTTPacket_equals(topicName, (char *)c->messageHandlers[i].topicFilter) ||
                isTopicMatched((char *)c->messageHandlers[i].topicFilter, topicName)))
        {
            if (c->messageHandlers[i].callback != NULL)
            {
                MessageData md;
                NewMessageData(&md, topicName, message);
                c->messageHandlers[i].callback(c, &md);
                rc = PAHO_SUCCESS;
            }
        }
    }

    if (rc == PAHO_FAILURE && c->defaultMessageHandler != NULL)
    {
        MessageData md;
        NewMessageData(&md, topicName, message);
        c->defaultMessageHandler(c, &md);
        rc = PAHO_SUCCESS;
    }

    return rc;
}

static int MQTT_cycle(MQTTClient *c)
{
    // read the socket, see what work is due
    int packet_type = MQTTPacket_readPacket(c);

    int len = 0,
        rc = PAHO_SUCCESS;


    if (packet_type == -1)
    {
        rc = PAHO_FAILURE;
        goto exit;
    }

    switch (packet_type)
    {
    case CONNACK:
    case PUBACK:
    case SUBACK:
    {
        int count = 0, grantedQoS = -1;
        unsigned short mypacketid;

        if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, c->readbuf, c->readbuf_size) == 1)
            rc = grantedQoS; // 0, 1, 2 or 0x80

        if (rc != 0x80)
            rc = 0;

        break;
    }  
    case UNSUBACK:
    {
        unsigned short mypacketid;

        if (MQTTDeserialize_unsuback(&mypacketid, c->readbuf, c->readbuf_size) == 1)
            rc =  PAHO_SUCCESS;
        else
            rc =  PAHO_FAILURE;

        break;
    }
    case PUBLISH:
    {
        MQTTString topicName;
        MQTTMessage msg;
        int intQoS;
        if (MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
                                    (unsigned char **)&msg.payload, (int *)&msg.payloadlen, c->readbuf, c->readbuf_size) != 1)
            goto exit;
        msg.qos = (enum QoS)intQoS;
        deliverMessage(c, &topicName, &msg);
        if (msg.qos != QOS0)
        {
            if (msg.qos == QOS1)
                len = MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
            else if (msg.qos == QOS2)
                len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREC, 0, msg.id);
            if (len <= 0)
                rc = PAHO_FAILURE;
            else
                rc = sendPacket(c, len);
            if (rc == PAHO_FAILURE)
                goto exit; // there was a problem
        }
        break;
    }
    case PUBREC:
    {
        unsigned short mypacketid;
        unsigned char dup, type;
        if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
            rc = PAHO_FAILURE;
        else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREL, 0, mypacketid)) <= 0)
            rc = PAHO_FAILURE;
        else if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the PUBREL packet
            rc = PAHO_FAILURE; // there was a problem
        if (rc == PAHO_FAILURE)
            goto exit; // there was a problem
        break;
    }
    case PUBCOMP:
        break;
    case PINGRESP:
        c->tick_ping = rt_tick_get();
        break;
    }

exit:
    return rc;
}

static int MQTT_local_send(MQTTClient *c, const void *data, int len)
{
    int send_len;

    send_len = write(c->pub_pipe[1], data, len);

    return send_len;
}

/*
MQTT_CMD:
"DISCONNECT"
*/
int MQTT_CMD(MQTTClient *c, const char *cmd)
{
    char *data = 0;
    int cmd_len, len;
    int rc = PAHO_FAILURE;

    if (!c->isconnected)
        goto _exit;

    cmd_len = strlen(cmd) + 1;
    if (cmd_len >= sizeof(MQTTMessage))
    {
        LOG_E("Commond is too long %d:%d.", cmd_len, sizeof(MQTTMessage));
        goto _exit;
    }

    data = rt_malloc(cmd_len);
    if (!data)
        goto _exit;

    strcpy(data, cmd);
    len = MQTT_local_send(c, data, cmd_len);
    if (len == cmd_len)
    {
        rc = 0;
    }

_exit:
    if (data)
        rt_free(data);

    return rc;
}

/**
 * This function publish message to specified mqtt topic.
 * [MQTTMessage] + [payload] + [topic] + '\0'
 *
 * @param c the pointer of MQTT context structure
 * @param topicFilter topic filter name
 * @param message the pointer of MQTTMessage structure
 *
 * @return the error code, 0 on subscribe successfully.
 */
int MQTTPublish(MQTTClient *c, const char *topicName, MQTTMessage *message)
{
    int rc = PAHO_FAILURE;
    int len, msg_len;
    char *data = 0;

    if (!c->isconnected)
        goto exit;

    msg_len = sizeof(MQTTMessage) + message->payloadlen + strlen(topicName) + 1;
    data = rt_malloc(msg_len);
    if (!data)
        goto exit;

    memcpy(data, message, sizeof(MQTTMessage));
    memcpy(data + sizeof(MQTTMessage), message->payload, message->payloadlen);
    strcpy(data + sizeof(MQTTMessage) + message->payloadlen, topicName);


    len = MQTT_local_send(c, data, msg_len);
    if (len == msg_len)
    {
        rc = PAHO_SUCCESS;
    }

    if (c->isblocking && c->pub_mutex)
    {
        if(rt_mutex_take(c->pub_mutex, 5 * RT_TICK_PER_SECOND) < 0)
        {
            rc = PAHO_FAILURE;
        }
    }

exit:
    if (data)
        rt_free(data);

    return rc;
}

static struct rt_pipe_device *mqtt_pipe_init(int filds[2])
{
    char dname[8];
    char dev_name[32];
    static int pipeno = 0;
    struct rt_pipe_device *pipe = RT_NULL;

    rt_snprintf(dname, sizeof(dname), "MQTT%d", pipeno++);
    pipe = rt_pipe_create(dname, PIPE_BUFSZ);
    if (pipe == RT_NULL)
    {
        LOG_E("create mqtt pipe fail\n");
        return RT_NULL;
    }

    rt_snprintf(dev_name, sizeof(dev_name), "/dev/%s", dname);
    filds[0] = open(dev_name, O_RDONLY, 0);
    if (filds[0] < 0)
    {
        LOG_E("pipe_read_fd open failed\n");
        return RT_NULL;
    }

    filds[1] = open(dev_name, O_WRONLY, 0);
    if (filds[1] < 0)
    {
        close(filds[1]);
        LOG_E("pipe_write_fd open failed\n");
        return RT_NULL;
    }
    
    return pipe;
}

static void paho_mqtt_thread(void *param)
{
    MQTTClient *c = (MQTTClient *)param;
    int i, rc, len;
    int rc_t = 0;

    /* create publish pipe */
    c->pipe_device = mqtt_pipe_init(c->pub_pipe);
    if (c->pipe_device == RT_NULL)
    {
        LOG_E("Create publish pipe device error.");
        goto _mqtt_exit;
    }

_mqtt_start:
    if (c->connect_callback)
    {
        c->connect_callback(c);
    }

    rc = net_connect(c);
    if (rc != 0)
    {
        LOG_E("Net connect error(%d).", rc);
        goto _mqtt_restart;
    }

    rc = MQTTConnect(c);
    if (rc != 0)
    {
        LOG_E("MQTT connect error(%d): %s.", rc, MQTTSerialize_connack_string(rc));
        goto _mqtt_restart;
    }

    LOG_I("MQTT server connect success.");

    for (i = 0; i < MAX_MESSAGE_HANDLERS; i++)
    {
        const char *topic = c->messageHandlers[i].topicFilter;
        enum QoS qos = c->messageHandlers[i].qos;

        if (topic == RT_NULL)
            continue;

        rc = MQTTSubscribe(c, topic, qos);
        LOG_I("Subscribe #%d %s %s!", i, topic, (rc < 0) || (rc == 0x80) ? ("fail") : ("OK"));

        if (rc != 0)
        {
            if (rc == 0x80)
            {
                LOG_E("QoS(%d) config err!", qos);
            }
            goto _mqtt_disconnect;
        }
    }

    if (c->online_callback)
    {
        c->online_callback(c);
    }

    c->tick_ping = rt_tick_get();
    while (1)
    {
        int res;
        rt_tick_t tick_now;
        fd_set readset;
        struct timeval timeout;

        tick_now = rt_tick_get();
        if (((tick_now - c->tick_ping) / RT_TICK_PER_SECOND) > (c->keepAliveInterval - 5))
        {
            timeout.tv_sec = 1;
            //LOG_D("tick close to ping.");
        }
        else
        {
            timeout.tv_sec = c->keepAliveInterval - 10 - (tick_now - c->tick_ping) / RT_TICK_PER_SECOND;
            //LOG_D("timeount for ping: %d", timeout.tv_sec);
        }
        timeout.tv_usec = 0;

        FD_ZERO(&readset);
        FD_SET(c->sock, &readset);
        FD_SET(c->pub_pipe[0], &readset);

        /* int select(maxfdp1, readset, writeset, exceptset, timeout); */
        res = select(((c->pub_pipe[0] > c->sock) ? c->pub_pipe[0] : c->sock) + 1,
                     &readset, RT_NULL, RT_NULL, &timeout);
        if (res == 0)
        {
            len = MQTTSerialize_pingreq(c->buf, c->buf_size);
            rc = sendPacket(c, len);
            if (rc != 0)
            {
                LOG_E("[%d] send ping rc: %d ", rt_tick_get(), rc);
                goto _mqtt_disconnect;
            }

            /* wait Ping Response. */
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;

            FD_ZERO(&readset);
            FD_SET(c->sock, &readset);

            res = select(c->sock + 1, &readset, RT_NULL, RT_NULL, &timeout);
            if (res <= 0)
            {
                LOG_E("[%d] wait Ping Response res: %d", rt_tick_get(), res);
                goto _mqtt_disconnect;
            }
        } /* res == 0: timeount for ping. */

        if (res < 0)
        {
            LOG_E("select res: %d", res);
            goto _mqtt_disconnect;
        }

        if (FD_ISSET(c->sock, &readset))
        {
            //LOG_D("sock FD_ISSET");
            rc_t = MQTT_cycle(c);
            //LOG_D("sock FD_ISSET rc_t : %d", rc_t);
            if (rc_t < 0)    goto _mqtt_disconnect;

            continue;
        }

        if (FD_ISSET(c->pub_pipe[0], &readset))
        {
            MQTTMessage *message;
            MQTTString topic = MQTTString_initializer;

            //LOG_D("pub_sock FD_ISSET");

            len = read(c->pub_pipe[0], c->readbuf, c->readbuf_size);

            if (len < sizeof(MQTTMessage))
            {
                c->readbuf[len] = '\0';
                LOG_D("pub_sock recv %d byte: %s", len, c->readbuf);

                if (strcmp((const char *)c->readbuf, "DISCONNECT") == 0)
                {
                    goto _mqtt_disconnect_exit;
                }

                continue;
            }

            message = (MQTTMessage *)c->readbuf;
            message->payload = c->readbuf + sizeof(MQTTMessage);
            topic.cstring = (char *)c->readbuf + sizeof(MQTTMessage) + message->payloadlen;
            //LOG_D("pub_sock topic:%s, payloadlen:%d", topic.cstring, message->payloadlen);

            len = MQTTSerialize_publish(c->buf, c->buf_size, 0, message->qos, message->retained, message->id,
                                        topic, (unsigned char *)message->payload, message->payloadlen);
            if (len <= 0)
            {
                LOG_D("MQTTSerialize_publish len: %d", len);
                goto _mqtt_disconnect;
            }

            if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the subscribe packet
            {
                LOG_D("MQTTSerialize_publish sendPacket rc: %d", rc);
                goto _mqtt_disconnect;
            }

            if (c->isblocking && c->pub_mutex)
            {
                rt_mutex_release(c->pub_mutex);
            }
        } /* pbulish sock handler. */
    } /* while (1) */

_mqtt_disconnect:
    MQTTDisconnect(c);
_mqtt_restart:
    if (c->offline_callback)
    {
        c->offline_callback(c);
    }

    net_disconnect(c);
    rt_thread_delay(c->reconnect_interval > 0 ? 
        rt_tick_from_millisecond(c->reconnect_interval) : RT_TICK_PER_SECOND * 5);
    LOG_D("restart!");
    goto _mqtt_start;

_mqtt_disconnect_exit:
    MQTTDisconnect(c);
    net_disconnect_exit(c);

_mqtt_exit:
    LOG_I("MQTT server is disconnected.");

    return;
}

/**
 * This function start a mqtt worker thread.
 *
 * @param client the pointer of MQTT context structure
 *
 * @return the error code, 0 on start successfully.
 */
int paho_mqtt_start(MQTTClient *client)
{
    rt_thread_t tid = RT_NULL;
    static uint8_t counts = 0;
    char pub_name[RT_NAME_MAX], thread_name[RT_NAME_MAX];

    /* create publish mutex */
    rt_memset(pub_name, 0x00, sizeof(pub_name));
    rt_snprintf(pub_name, RT_NAME_MAX, "pmtx%d", counts);
    client->pub_mutex = rt_mutex_create(pub_name, RT_IPC_FLAG_FIFO);
    if (client->pub_mutex == RT_NULL)
    {
        LOG_E("Create publish mutex error.");
        return PAHO_FAILURE;
    }

    rt_memset(thread_name, 0x00, sizeof(thread_name));
    rt_snprintf(thread_name, RT_NAME_MAX, "mqtt%d", counts++);
    tid = rt_thread_create( thread_name,
                            paho_mqtt_thread, (void *) client,      // fun, parameter
                            RT_PKG_MQTT_THREAD_STACK_SIZE,          // stack size
                            RT_THREAD_PRIORITY_MAX / 3, 2 );         //priority, tick      
    if (tid)
    {
        rt_thread_startup(tid);
    }

    return PAHO_SUCCESS;
}

/**
 * This function stop MQTT worker thread and free MQTT client object.
 *
 * @param client the pointer of MQTT context structure
 *
 * @return the error code, 0 on start successfully.
 */
int paho_mqtt_stop(MQTTClient *client)
{
    return MQTT_CMD(client, "DISCONNECT");
}

/**
 * This function send an MQTT subscribe packet and wait for suback before returning.
 *
 * @param client the pointer of MQTT context structure
 * @param qos MQTT Qos type, only support QOS1
 * @param topic topic filter name
 * @param callback the pointer of subscribe topic receive data function
 *
 * @return the error code, 0 on start successfully.
 */
int paho_mqtt_subscribe(MQTTClient *client, enum QoS qos, const char *topic, subscribe_cb callback)
{
    int i, lenght, rc = PAHO_SUCCESS;
    int qos_sub = qos;
    MQTTString topicFilters = MQTTString_initializer;
    topicFilters.cstring = (char *)topic;

    RT_ASSERT(client);
    RT_ASSERT(topic);

    if (qos != QOS1)
    {
        LOG_E("Not support Qos(%d) config, only support Qos(d).", qos, QOS1);
        return PAHO_FAILURE;
    }

    for (i = 0; i < MAX_MESSAGE_HANDLERS ; ++i)
    {
        if (client->messageHandlers[i].topicFilter && 
                rt_strncmp(client->messageHandlers[i].topicFilter, topic, rt_strlen(topic)) == 0)
        {
            LOG_D("MQTT client topic(%s) is already subscribed.", topic);
            return PAHO_SUCCESS;
        }
    }

    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (client->messageHandlers[i].topicFilter)
        {
            continue;
        }

        lenght = MQTTSerialize_subscribe(client->buf, client->buf_size, 0, getNextPacketId(client), 1, &topicFilters, &qos_sub);
        if (lenght <= 0)
        {
            LOG_E("Subscribe #%d %s failed!", i, topic);
            client->isconnected = 0;
            goto _exit;
        }

        rc = sendPacket(client, lenght);
        if (rc != PAHO_SUCCESS) 
        {
            LOG_E("Subscribe #%d %s failed!", i, topic);
            client->isconnected = 0;
            goto _exit;
        }

        client->messageHandlers[i].qos = qos;
        client->messageHandlers[i].topicFilter = rt_strdup((char *)topic);
        if (callback)
        {
            client->messageHandlers[i].callback = callback;
        }

        LOG_I("Subscribe #%d %s OK!", i, topic);
        goto _exit;
    }

    /* check subscribe numble support */
    if (i >= MAX_MESSAGE_HANDLERS)
    {
        LOG_E("Subscribe MAX_MESSAGE_HANDLERS size(%d) is not enough!", MAX_MESSAGE_HANDLERS);
        rc = PAHO_FAILURE;
        goto _exit;
    }

_exit:
    return rc;
}

/**
 * This function send an MQTT unsubscribe packet and wait for unsuback before returning.
 *
 * @param client the pointer of MQTT context structure
 * @param topic topic filter name
 *
 * @return the error code, 0 on start successfully.
 */
int paho_mqtt_unsubscribe(MQTTClient *client, const char *topic)
{
    int i, length, rc = PAHO_SUCCESS;
    MQTTString topicFilter = MQTTString_initializer;
    topicFilter.cstring = (char *)topic;

    RT_ASSERT(client);
    RT_ASSERT(topic);

    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (client->messageHandlers[i].topicFilter == RT_NULL || 
                rt_strncmp(client->messageHandlers[i].topicFilter, topic, rt_strlen(topic)) != 0)
        {
            continue;
        }

        length = MQTTSerialize_unsubscribe(client->buf, client->buf_size, 0, getNextPacketId(client), 1, &topicFilter);
        if (length <= 0)
        {
            LOG_E("Unubscribe #%d %s failed!", i, topic);
            client->isconnected = 0;
            goto _exit;
        }

        rc = sendPacket(client, length);
        if (rc != PAHO_SUCCESS)
        {
            LOG_E("Unubscribe #%d %s failed!", i, topic);
            client->isconnected = 0;
            goto _exit;
        }

        /* clear message handler */
        if (client->messageHandlers[i].topicFilter)
        {
            rt_free(client->messageHandlers[i].topicFilter);
            client->messageHandlers[i].topicFilter = RT_NULL;
        }
        client->messageHandlers[i].callback = RT_NULL; 

        LOG_I("Unsubscribe #%d %s OK!", i, topic);
        goto _exit;
    }

    /* check subscribe topic */
    if (i >= MAX_MESSAGE_HANDLERS)
    {
        LOG_E("Unsubscribe topic(%s) is not exist!", topic);
        rc = PAHO_FAILURE;
        goto _exit;
    }

_exit:
    return rc;
}

/**
 * This function publish message to specified mqtt topic.
 *
 * @param c the pointer of MQTT context structure
 * @param qos MQTT QOS type, only support QOS1
 * @param topic topic filter name
 * @param msg_str the pointer of MQTTMessage structure
 *
 * @return the error code, 0 on subscribe successfully.
 */
int paho_mqtt_publish(MQTTClient *client, enum QoS qos, const char *topic, const char *msg_str)
{
    MQTTMessage message;

    if (qos != QOS1)
    {
        LOG_E("Not support Qos(%d) config, only support Qos(d).", qos, QOS1);
        return PAHO_FAILURE;
    }

    message.qos = qos;
    message.retained = 0;
    message.payload = (void *)msg_str;
    message.payloadlen = rt_strlen(message.payload);

    return MQTTPublish(client, topic, &message);
}

/**
 * This function control MQTT client configure, such as connect timeout, reconnect interval.
 *
 * @param c the pointer of MQTT context structure
 * @param cmd control configure type, 'mqttControl' enumeration shows the supported configure types.
 * @param arg the pointer of argument
 *
 * @return the error code, 0 on subscribe successfully.
 */
int paho_mqtt_control(MQTTClient *client, int cmd, void *arg)
{
    RT_ASSERT(client);
    RT_ASSERT(arg);

    switch (cmd)
    {
        case MQTT_CTRL_SET_CONN_TIMEO:
            client->connect_timeout = *(int *)arg;
            break;

        case MQTT_CTRL_SET_RECONN_INTERVAL:
            client->reconnect_interval = *(int *)arg;
            break;
        
        case MQTT_CTRL_SET_KEEPALIVE_INTERVAL:
            client->keepAliveInterval = *(unsigned int *)arg;
            break;

        case MQTT_CTRL_PUBLISH_BLOCK:
            client->isblocking = *(int *)arg;
            break;
        
        default:
            LOG_E("Input control commoand(%d) error.", cmd);
            break;
    }

    return PAHO_SUCCESS;
}

