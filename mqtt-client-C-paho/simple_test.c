#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://broker.emqx.io:1883"
#define USERNAME    "emqx"
#define PASSWORD    "public"
#define CLIENTID    "c-client-test"
#define QOS         0
#define TOPIC       "emqx/c-test"
#define TIMEOUT     10000L

int main(int argc, char *argv[]) {
    int rc;
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    
    MQTTClient_create(&client, ADDRESS, CLIENTID, 0, NULL);
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    } else {
        printf("Connected to MQTT Broker!\n");
    }
    
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return 0;
}
