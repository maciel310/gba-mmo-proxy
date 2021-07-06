#include <stdlib.h>
#include <string.h>

#include "account.h"
#include "udp.h"

#ifdef LOCAL
  #define COOKIE_FILE "gba-mmo-cookie-local"
#else
  #define COOKIE_FILE "gba-mmo-cookie"
#endif

char* read_cookie_file() {
  char* buffer = NULL;
  size_t len;

  FILE *fp = fopen(COOKIE_FILE, "r");
  if (fp == NULL) {
    return "";
  }
  getdelim(&buffer, &len, '\0', fp);
  fclose(fp);

  return buffer;
}

bool login() {
  char* cookie = read_cookie_file();

  if (strlen(cookie) == 0) {
    return false;
  } else {
    char login_message[100];
    sprintf(login_message, "login~~%s", cookie);
    udp_send_string(login_message);
    
    char response_buffer[512];
    int rec_size = read_server_update(response_buffer, 512);

    return strcmp(response_buffer, "success") == 0;
  }
}

bool create_account(char* name, char* email) {
  char create_message[512];
  sprintf(create_message, "create~~%s~~%s", name, email);

  udp_send_string(create_message);

  char response_buffer[512];
  int rec_size = read_server_update(response_buffer, 512);

  if (strncmp("success~~", response_buffer, strlen("success~~")) == 0) {
    char* cookie = response_buffer;
    cookie += strlen("success~~");

    FILE *fp = fopen(COOKIE_FILE, "w");
    if (fp == NULL) {
      printf("COULD NOT WRITE COOKIE FILE\n");
      return false;
    }
    fputs(cookie, fp);
    fclose(fp);

    return true;
  }

  return false;
}
