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

  uint16_t x = 0;
  uint16_t y = 0;
  uint32_t xy = 0;

  uint32_t otherXY = 0;

  while (1) {
    usleep(50000);
    xy = Spi32(otherXY);
    if (xy != 0) {
      x = (xy >> 16);
      y = (xy & 0x00FF);
      otherXY = udp_send_location(x, y);
    }
  }

  return 0;
}
