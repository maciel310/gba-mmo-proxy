#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include "string.h"

#include "udp.h"

#define ADDRESS "192.168.1.58"
#define PORT 5465

int sockfd;
struct sockaddr_in serveraddr;
int serverlen;

void udp_init() {
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  struct hostent *server = gethostbyname(ADDRESS);

  serveraddr.sin_family = AF_INET;
  memcpy((char *)&serveraddr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
  serveraddr.sin_port = htons(PORT);
  serverlen = sizeof(serveraddr);

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 50 * 1000;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

void udp_send_location(uint32_t *buffer, uint32_t len) {
  for (int i = 0; i < len; i++) {
    buffer[i] = htonl(buffer[i]);
  }
  sendto(sockfd, buffer, len, 0, (struct sockaddr *)&serveraddr, serverlen);
}

int read_server_update(char *buffer, size_t len) {
  int n = recvfrom(sockfd, buffer, len, 0, (struct sockaddr *)&serveraddr, &serverlen);
  buffer[n] = '\0';
  return n;
}
