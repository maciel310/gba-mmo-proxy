#include <stdio.h>
#include "string.h"

#include "mqtt.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "maciel310"
#define TOPIC       "maciel310/testing/location"

MQTTClient client;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;

void connlost(void *context, char *cause) {
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

void mqtt_init() {
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;

  MQTTClient_setCallbacks(client, NULL, connlost, NULL, NULL);
  if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
    printf("Failed to connect");
  }
}

void mqtt_send_location(uint16_t x, uint16_t y) {
	char payload[30];
	sprintf(payload, "%d,%d", x, y);

  pubmsg.payload = payload;
  pubmsg.payloadlen = strlen(payload);
  pubmsg.qos = 0;
  pubmsg.retained = 0;
  MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
}
