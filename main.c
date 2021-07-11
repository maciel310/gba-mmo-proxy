#include <stdio.h>
#include <stdint.h>
#include <wiringPiSPI.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "account.h"
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
    udp_send_location(incoming_buffer, expected_length * 4);

    expected_length = 0;
    incoming_buffer_position = 0;
  }
}

int main(int argc, char* argv[]) {
  udp_init();
  if (!login()) {
    printf("Welcome to GBA-MMO! To create an account, first enter your email:\n");
    char *email = NULL;
    size_t email_len = 0;
    getline(&email, &email_len, stdin);

    printf("\nEnter your character name:\n");
    char *name = NULL;
    size_t name_len = 0;
    getline(&name, &name_len, stdin);

    // Remove trailing new line
    name[strlen(name)-1] = '\0';
    email[strlen(email)-1] = '\0';

    printf("\n\nCreating account...\n");
    if (!create_account(name, email)) {
      printf("Failed to create account!\n\n");
      return 0;
    }

    if (!login()) {
      printf("Failed to log in after creating account!\n\n");
      return 0;
    }

    printf("\nThanks for joining!\n\n");
  }

  printf("\nIf you haven't already, join the discord at: https://discord.gg/B3wJYnUswN\n\n");

  multiboot("gba_mb.gba");

  uint32_t buffer_len = 512;
  char buffer[512];
  while (1) {
    int rec_size = read_server_update(buffer, buffer_len);
    if (rec_size > 0) {
      uint32_t d;

      transfer((rec_size + 4) | 0xBEEF0000);
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
      transfer(0xDEADBEEF);
      usleep(1000);
    }
  }

  return 0;
}
