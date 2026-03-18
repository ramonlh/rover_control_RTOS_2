#ifndef SERVIDOR_WEBSOCKET_H
#define SERVIDOR_WEBSOCKET_H

#include <Arduino.h>
#include <WebSocketsServer.h>

void init_websocket();
void sendRadarData();
void websocket_fail_safe_check();
void task_websockets(void *pvParameters);

#endif