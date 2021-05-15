#include <stdio.h>
#include <stdint.h>
#include <wiringPiSPI.h>
#include <sys/stat.h>
#include <unistd.h>

#include "multiboot.h"
#include "udp.h"

uint32_t incoming_buffer[30];
uint32_t incoming_buffer_position = 0;
uint32_t expected_length = 0;
void transfer(uint32_t out) {
  uint32_t data = Spi32(out);

  if (expected_length == 0) {
    expected_length = data;
    incoming_buffer_position = 0;
    return;
  }
  incoming_buffer[incoming_buffer_position++] = data;

  if (incoming_buffer_position >= expected_length) {
    for (int i = 0; i <= expected_length; i++) {
      printf("%08x", (unsigned long) incoming_buffer[i]);
    }
    printf("\n");
    udp_send_location(incoming_buffer, expected_length * 4);

    expected_length = 0;
    incoming_buffer_position = 0;
  }
}

int main(int argc, char* argv[]) {
  multiboot("gba_mb.gba");

  udp_init();
  udp_send_location(incoming_buffer, 1);

  uint32_t keepalive_counter = 0;

  uint32_t buffer_len = 512;
  char buffer[512];
  while (1) {
    int rec_size = read_server_update(buffer, buffer_len);
    if (rec_size > 0) {
      uint32_t d;

      transfer(rec_size);
      usleep(1000);

      for (int i = 0; i < rec_size; i++) {
        d =
          (buffer[i++] << 24) |
          (buffer[i++] << 16) |
          (buffer[i++] << 8) |
          (buffer[i]);
        transfer(d);
        usleep(1000);
      }
    } else {
      keepalive_counter++;
      if (keepalive_counter > 100) {
        printf("Sending keepalive\n");
        udp_send_location(incoming_buffer, 1);
        keepalive_counter = 0;
      }
    }
  }

  return 0;
}
