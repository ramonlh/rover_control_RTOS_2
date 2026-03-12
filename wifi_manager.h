
#ifndef WIFI_MANAGER_CUSTOM_H
#define WIFI_MANAGER_CUSTOM_H

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

extern bool wm_nonblocking;
extern WiFiManager wm;
extern WiFiManagerParameter custom_field;

String getParam(String name);
void saveParamCallback();
void checkButton();
void init_wifi_manager();
void wifimanager_loop();

#endif