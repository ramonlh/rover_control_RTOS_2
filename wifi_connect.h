
#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <WiFi.h>

extern const char *ssid;
extern const char *password;

extern const char *ssidAP;
extern const char *passwordAP;

void init_wifi();

#endif