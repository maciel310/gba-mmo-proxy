#include <stdio.h>
#include <stdint.h>
#include <wiringPiSPI.h>
#include <sys/stat.h>
#include <unistd.h>

#include "multiboot.h"
#include "mqtt.h"

int main(int argc, char* argv[]) {
  mqtt_init();

  multiboot("gba_mb.gba");

  uint16_t x = 0;
  uint16_t y = 0;
  uint32_t xy = 0;
  while (1) {
    usleep(50000);
    xy = Spi32(0);
    if (xy != 0) {
      x = (xy >> 16);
      y = (xy & 0x00FF);
      mqtt_send_location(x, y);
    }
  }
}
