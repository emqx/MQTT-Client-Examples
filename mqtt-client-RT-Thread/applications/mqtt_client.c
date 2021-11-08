#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <rtthread.h>
#include <ulog.h>
#include "paho_mqtt.h"
#include "sht20.h"
/**
 * MQTT URI farmat:
 * domain mode
 * tcp://iot.eclipse.org:1883
 *
 * ipv4 mode
 * tcp://192.168.10.1:1883
 * ssl://192.168.10.1:1884
 *
 * ipv6 mode
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * ssl://[fe80::20c:29ff:fe9a:a07e]:1884
 */

#define EMQX_Cloud_Professional_Version 0
#define EMQX_Cloud_TLS_SSL 1

#if !EMQX_Cloud_Professional_Version

#if EMQX_Cloud_TLS_SSL
#define MQTT_BROKER_URI         "ssl://ha9a03b0.cn-shenzhen.emqx.cloud:12699"
#else
#define MQTT_BROKER_URI         "tcp://ha9a03b0.cn-shenzhen.emqx.cloud:11762"
#endif
#else

#if EMQX_Cloud_TLS_SSL
#define MQTT_BROKER_URI         "ssl://oba9d641.emqx.cloud:8883"
#else
#define MQTT_BROKER_URI         "tcp://oba9d641.emqx.cloud:1883"
#endif

#endif

#define MQTT_CLIENTID_PREFIX    "rtthread-mqtt"
#define MQTT_USERNAME           "test"
#define MQTT_PASSWORD           "testemq"
#define MQTT_SUBTOPIC           "/emqx/mqtt/rep"
#define MQTT_PUBTOPIC           "/emqx/mqtt/req"
#define MQTT_WILLMSG            "Goodbye!"
#define MQTT_QOS                1
#define MQTT_PUB_SUB_BUF_SIZE   1024

#define CMD_INFO                "'mqtt_ctrl <start|stop>'"
#define TEST_DATA_SIZE          256
#define PUB_CYCLE_TM            1000

#define I2C_NAME    "i2c3"

static rt_thread_t pub_thread_tid = RT_NULL;

static char *pub_data = RT_NULL;

/* define MQTT client context */
static MQTTClient client;

static rt_uint32_t pub_count = 0, sub_count = 0;
static int recon_count = -1;
static int is_started = 0;

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    sub_count++;
    *((char *) msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    rt_kprintf("mqtt sub callback[%u]: \n topic: %.*s \n message %.*s", sub_count, msg_data->topicName->lenstring.len,
            msg_data->topicName->lenstring.data, msg_data->message->payloadlen, (char *) msg_data->message->payload);
}

static void mqtt_connect_callback(MQTTClient *c)
{
    rt_kprintf(" mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    recon_count++;
    rt_kprintf(" mqtt_online_callback[%d]!", recon_count);
}

static void mqtt_offline_callback(MQTTClient *c)
{
    rt_kprintf(" mqtt_offline_callback!");
}

/**
 * This function create and config a mqtt client.
 *
 * @param void
 *
 * @return none
 */
static void mqtt_create(void)
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;

    static char client_id[50] = { 0 };

    rt_memset(&client, 0, sizeof(MQTTClient));

    /* config MQTT context param */
    {
        client.uri = MQTT_BROKER_URI;

        /* config connect param */
        memcpy(&client.condata, &condata, sizeof(condata));
        rt_snprintf(client_id, sizeof(client_id), "%s%d", MQTT_CLIENTID_PREFIX, rt_tick_get());
        client.condata.clientID.cstring = client_id;
        client.condata.keepAliveInterval = 60;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* config MQTT will param. */
        client.condata.willFlag = 1;
        client.condata.will.qos = MQTT_QOS;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = MQTT_PUBTOPIC;
        client.condata.will.message.cstring = MQTT_WILLMSG;

        /* malloc buffer. */
        client.buf_size = client.readbuf_size = MQTT_PUB_SUB_BUF_SIZE;
        client.buf = rt_malloc(client.buf_size);
        client.readbuf = rt_malloc(client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            rt_kprintf("no memory for MQTT client buffer!\n");
            goto _exit;
        }

        /* set event callback function */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* set subscribe table and event callback */
        client.messageHandlers[0].topicFilter = rt_strdup(MQTT_SUBTOPIC);
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = MQTT_QOS;

        /* set default subscribe event callback */
        client.defaultMessageHandler = mqtt_sub_callback;
    }

    /* run mqtt client */
    paho_mqtt_start(&client);

    return;

    _exit: if (client.buf)
    {
        rt_free(client.buf);
        client.buf = RT_NULL;
    }
    if (client.readbuf)
    {
        rt_free(client.readbuf);
        client.readbuf = RT_NULL;
    }
    return;
}

static void thread_pub(void *parameter)
{
    pub_data = rt_malloc(TEST_DATA_SIZE * sizeof(char));
    if (!pub_data)
    {
        rt_kprintf("no memory for pub_data\n");
        return;
    }

    sht20_device_t dev = sht20_init(I2C_NAME);
    rt_kprintf("sht20_device_t init:   %p\n", dev);

    rt_kprintf("test start at '%d'\r\n", time((time_t *) RT_NULL));
    float humidity = 0.0, temperature = 0.0;

    while (1)
    {
        humidity = sht20_read_humidity(dev);
        temperature = sht20_read_temperature(dev);

        snprintf(pub_data, TEST_DATA_SIZE, "{\n"
                "  \"seq\": \"%u\",\n"
                "  \"time\": \"%ld\", \n"
                "  \"humidity\": \"%.2f\",\n"
                "  \"temperature\": \"%.2f\"\n"
                "}", pub_count, time((time_t *) RT_NULL), humidity, temperature);

        rt_kprintf("%s\n", pub_data);

        if (paho_mqtt_publish(&client, QOS1, MQTT_PUBTOPIC, pub_data) == 0)
        {
            ++pub_count;
        }

        rt_thread_delay(PUB_CYCLE_TM);
    }
    sht20_deinit(dev);
}

void mqtt_client_start(void)
{
    if (is_started)
    {
        return;
    }

    mqtt_create();

    while (!client.isconnected)
    {
        rt_kprintf("Waiting for mqtt connection...\n");
        rt_thread_delay(1000);
    }

    pub_thread_tid = rt_thread_create("pub_thread", thread_pub, RT_NULL, 1024, 8, 100);
    if (pub_thread_tid != RT_NULL)
    {
        rt_thread_startup(pub_thread_tid);
    }

    is_started = 1;

    return;
}

void mqtt_client_stop(void)
{
    MQTTClient *local_client = &client;

    if (pub_thread_tid)
    {
        rt_thread_delete(pub_thread_tid);
    }

    if (pub_data)
    {
        rt_free(pub_data);
        pub_data = RT_NULL;
    }

    if (local_client)
    {
        paho_mqtt_stop(local_client);
    }

    pub_count = sub_count = recon_count = 0;
    is_started = 0;

    rt_kprintf("==== MQTT Stability test stop ====\n");
}

static void mqtt_ctrl(uint8_t argc, char **argv)
{
    if (argc >= 2)
    {
        if (!strcmp(argv[1], "start"))
        {

            mqtt_client_start();
        }
        else if (!strcmp(argv[1], "stop"))
        {
            mqtt_client_stop();
        }
        else
        {
            rt_kprintf("Please input "CMD_INFO"\n");
        }
    }
    else
    {
        rt_kprintf("Please input "CMD_INFO"\n");
    }
}
MSH_CMD_EXPORT(mqtt_ctrl, MQTT ctrl CMD_INFO);

