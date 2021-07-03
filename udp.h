
#include <stdint.h>
#include "MQTTClient.h"

void udp_init();
void udp_send_string(char *buffer);
void udp_send_location(uint32_t *buffer, uint32_t len);
int read_server_update(char *buffer, size_t len);
