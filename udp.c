#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "string.h"

#include "udp.h"

#define ADDRESS "192.168.1.58"
#define PORT 5465

int sockfd;
struct sockaddr_in serveraddr;

void udp_init() {
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  struct hostent *server = gethostbyname(ADDRESS);

  serveraddr.sin_family = AF_INET;
  memcpy((char *)&serveraddr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
  serveraddr.sin_port = htons(PORT);
}

uint32_t udp_send_location(uint16_t x, uint16_t y) {
	char payload[30];
	sprintf(payload, "%d,%d", x, y);

  int serverlen = sizeof(serveraddr);
  sendto(sockfd, payload, strlen(payload), 0, (struct sockaddr *)&serveraddr, serverlen);

  int n = recvfrom(sockfd, payload, 30, MSG_WAITALL, (struct sockaddr *)&serveraddr, &serverlen);
  payload[n] = '\0';

  char *end;
  const long rx = strtol(payload, &end, 10);
  end++;
  const long ry = strtol(end, &end, 10);

  return (rx << 16) | ry;
}
