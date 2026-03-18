

#ifndef SERVIDOR_WEB_H
#define SERVIDOR_WEB_H

#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <FFat.h>

#include "defines.h"
#include "web_pages.h"
#include "wifi_connect.h"

extern WebServer webserver;
extern char other_ip[16];

String SendHTML();
String SendSetupHTML();

void send_200();
void send_204();

void handle_index();
void handle_NotFound();
void handle_image(const char* filename);
void handle_cochearriba();
void handle_cochetrasera();
void handle_cochelado();
void handle_GetConfig();
void handle_setup();
void handle_save_config();
void init_webserver();

#endif