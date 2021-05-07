
#include <stdint.h>
#include "MQTTClient.h"

void udp_init();
void udp_send_location(uint16_t x, uint16_t y);
int read_server_update(char *buffer, size_t len);
