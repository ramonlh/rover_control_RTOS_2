
#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <WiFi.h>


extern char ssid_buf[64];
extern char password_buf[64];
extern char ssidAP_buf[64];
extern char passwordAP_buf[64];

extern const char *ssid;
extern const char *password;
extern const char *ssidAP;
extern const char *passwordAP;

void init_wifi();

#endif