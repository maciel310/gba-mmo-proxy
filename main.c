#include <stdio.h>
#include <stdint.h>
#include <wiringPiSPI.h>
#include <sys/stat.h>
#include <unistd.h>

#include "multiboot.h"
#include "udp.h"

int main(int argc, char* argv[]) {
  multiboot("gba_mb.gba");

  udp_init();
  udp_send_location(12, 12);

  uint16_t x = 0;
  uint16_t y = 0;
  uint32_t xy = 0;

  uint32_t buffer_len = 512;
  char buffer[512];
  while (1) {
    int rec_size = read_server_update(buffer, buffer_len);
    if (rec_size > 0) {
      uint32_t d;

      Spi32(rec_size);
      usleep(1000);

      for (int i = 0; i < rec_size; i++) {
        d =
          (buffer[i++] << 24) |
          (buffer[i++] << 16) |
          (buffer[i++] << 8) |
          (buffer[i]);
        printf("%08x", d);
        Spi32(d);
        usleep(1000);
      }

      printf("\n");
    }
  }

  return 0;
}
