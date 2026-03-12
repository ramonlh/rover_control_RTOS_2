#include "servidor_websocket.h"
#include "defines.h"
#include "DHTesp.h"

#include <WiFi.h>
#include <ArduinoJson.h>

// Estos símbolos ya existen en otros módulos .h del proyecto
extern int tipo_mov;
extern int rover_speed;

extern int angulo_servo_1;
extern int angulo_servo_2;

extern int ang_radar;
extern int dist_radar;
extern int enviar_dist;

extern float angleX;
extern float angleY;
extern float angleZ;

extern TempAndHumidity valores_DHT11;

void rover_move(int d_trd, int d_tri, int d_ded, int d_dei,
                int s_trd, int s_tri, int s_ded, int s_dei);
void rover_stop();

namespace {
    constexpr size_t JSON_BUFFER_SIZE = 160;
    constexpr size_t MAX_WS_MESSAGE   = 192;

    constexpr int ROVER_SPEED_MIN = 500;
    constexpr int ROVER_SPEED_MAX = 4000;

    constexpr uint32_t WS_FAILSAFE_MS = 1000;

    WebSocketsServer webSocket(PORT_WEBSOCKET);
    uint32_t ultimo_comando_ms = 0;
    bool websocket_iniciado = false;
}

static int clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

static void marcar_actividad_comando()
{
    ultimo_comando_ms = millis();
}

static void scanWiFiNetworks(uint8_t cliente)
{
    int n = WiFi.scanNetworks();
    String ssidList = "{\"type\":\"ssids\",\"list\":[";
    for (int i = 0; i < n; i++) {
        if (i > 0) ssidList += ",";
        ssidList += "\"";
        ssidList += WiFi.SSID(i);
        ssidList += "\"";
    }
    ssidList += "]}";
    webSocket.sendTXT(cliente, ssidList);
}

static void enviar_estado_basico(uint8_t cliente)
{
    char buffer[JSON_BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer),
             "{\"type\":\"status\",\"speed\":%d,\"mov\":%d,\"cam1\":%d,\"cam2\":%d}",
             rover_speed, tipo_mov, angulo_servo_1, angulo_servo_2);
    webSocket.sendTXT(cliente, buffer);
}

static bool procesar_json_movimiento(const char* message)
{
    StaticJsonDocument<256> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, message);
    if (error) {
        return false;
    }

    if (!jsonDoc.containsKey("d1") || !jsonDoc.containsKey("d2") ||
        !jsonDoc.containsKey("d3") || !jsonDoc.containsKey("d4") ||
        !jsonDoc.containsKey("s1") || !jsonDoc.containsKey("s2") ||
        !jsonDoc.containsKey("s3") || !jsonDoc.containsKey("s4")) {
        return false;
    }

    int d_trd = jsonDoc["d1"].as<int>();
    int d_tri = jsonDoc["d2"].as<int>();
    int d_ded = jsonDoc["d3"].as<int>();
    int d_dei = jsonDoc["d4"].as<int>();

    int s_trd = jsonDoc["s1"].as<int>();
    int s_tri = jsonDoc["s2"].as<int>();
    int s_ded = jsonDoc["s3"].as<int>();
    int s_dei = jsonDoc["s4"].as<int>();

    rover_move(d_trd, d_tri, d_ded, d_dei, s_trd, s_tri, s_ded, s_dei);
    marcar_actividad_comando();
    return true;
}

static bool procesar_comando_texto(uint8_t cliente, const char* message)
{
    if      (strcmp(message, "f") == 0)    { tipo_mov = 1;  marcar_actividad_comando(); return true; }
    else if (strcmp(message, "r") == 0)    { tipo_mov = 2;  marcar_actividad_comando(); return true; }
    else if (strcmp(message, "fl") == 0)   { tipo_mov = 3;  marcar_actividad_comando(); return true; }
    else if (strcmp(message, "fr") == 0)   { tipo_mov = 4;  marcar_actividad_comando(); return true; }
    else if (strcmp(message, "rl") == 0)   { tipo_mov = 5;  marcar_actividad_comando(); return true; }
    else if (strcmp(message, "rr") == 0)   { tipo_mov = 6;  marcar_actividad_comando(); return true; }
    else if (strcmp(message, "rotl") == 0) { tipo_mov = 7;  marcar_actividad_comando(); return true; }
    else if (strcmp(message, "rotr") == 0) { tipo_mov = 8;  marcar_actividad_comando(); return true; }
    else if (strcmp(message, "latl") == 0) { tipo_mov = 9;  marcar_actividad_comando(); return true; }
    else if (strcmp(message, "latr") == 0) { tipo_mov = 10; marcar_actividad_comando(); return true; }

    else if (strcmp(message, "stop") == 0) {
        tipo_mov = 0;
        rover_stop();
        marcar_actividad_comando();
        return true;
    }

    else if (strcmp(message, "caml") == 0) {
        tipo_mov = 31;
        angulo_servo_1 = clamp_int(angulo_servo_1 + 1, 0, 180);
        marcar_actividad_comando();
        return true;
    }
    else if (strcmp(message, "camr") == 0) {
        tipo_mov = 32;
        angulo_servo_1 = clamp_int(angulo_servo_1 - 1, 0, 180);
        marcar_actividad_comando();
        return true;
    }
    else if (strcmp(message, "camup") == 0) {
        tipo_mov = 33;
        angulo_servo_2 = clamp_int(angulo_servo_2 - 1, 0, 180);
        marcar_actividad_comando();
        return true;
    }
    else if (strcmp(message, "camdn") == 0) {
        tipo_mov = 34;
        angulo_servo_2 = clamp_int(angulo_servo_2 + 1, 0, 180);
        marcar_actividad_comando();
        return true;
    }

    else if (strcmp(message, "DIF") == 0)  { tipo_mov = 41; marcar_actividad_comando(); return true; }
    else if (strcmp(message, "DDF") == 0)  { tipo_mov = 42; marcar_actividad_comando(); return true; }
    else if (strcmp(message, "TIF") == 0)  { tipo_mov = 43; marcar_actividad_comando(); return true; }
    else if (strcmp(message, "TDF") == 0)  { tipo_mov = 44; marcar_actividad_comando(); return true; }
    else if (strcmp(message, "DIR") == 0)  { tipo_mov = 45; marcar_actividad_comando(); return true; }
    else if (strcmp(message, "DDR") == 0)  { tipo_mov = 46; marcar_actividad_comando(); return true; }
    else if (strcmp(message, "TIR") == 0)  { tipo_mov = 47; marcar_actividad_comando(); return true; }
    else if (strcmp(message, "TDR") == 0)  { tipo_mov = 48; marcar_actividad_comando(); return true; }

    else if (strcmp(message, "speedmenos") == 0) {
        rover_speed = clamp_int(rover_speed - 500, ROVER_SPEED_MIN, ROVER_SPEED_MAX);
        Serial.println(rover_speed);
        return true;
    }
    else if (strcmp(message, "speedmas") == 0) {
        rover_speed = clamp_int(rover_speed + 500, ROVER_SPEED_MIN, ROVER_SPEED_MAX);
        Serial.println(rover_speed);
        return true;
    }

    else if (strcmp(message, "flash") == 0) {
        tipo_mov = 99;
        digitalWrite(pin_led_7colores, !digitalRead(pin_led_7colores));
        return true;
    }
    else if (strcmp(message, "luces") == 0) {
        tipo_mov = 99;
        digitalWrite(pin_led_izquierda, !digitalRead(pin_led_izquierda));
        digitalWrite(pin_led_derecha, !digitalRead(pin_led_derecha));
        return true;
    }
    else if (strcmp(message, "getssids") == 0) {
        tipo_mov = 99;
        scanWiFiNetworks(cliente);
        return true;
    }

    return false;
}

static void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type) {
        case WStype_CONNECTED:
            Serial.printf("WebSocket cliente %u conectado.\n", num);
            enviar_estado_basico(num);
            break;

        case WStype_DISCONNECTED:
            Serial.printf("WebSocket cliente %u desconectado.\n", num);
            if (webSocket.connectedClients() == 0) {
                tipo_mov = 0;
                rover_stop();
            }
            break;

        case WStype_TEXT: {
            if (length == 0 || length >= MAX_WS_MESSAGE) {
                webSocket.sendTXT(num, "{\"type\":\"error\",\"msg\":\"message_too_long\"}");
                return;
            }

            char message[MAX_WS_MESSAGE];
            memcpy(message, payload, length);
            message[length] = '\0';

            Serial.print("WS RX: ");
            Serial.println(message);

            if (procesar_json_movimiento(message)) return;
            if (procesar_comando_texto(num, message)) return;

            webSocket.sendTXT(num, "{\"type\":\"error\",\"msg\":\"unknown_command\"}");
            break;
        }

        default:
            break;
    }
}

void init_websocket()
{
    if (websocket_iniciado) return;

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    ultimo_comando_ms = millis();
    websocket_iniciado = true;

    Serial.println("Servidor WebSocket iniciado.");
}

void websocket_loop()
{
    if (!websocket_iniciado) return;

    webSocket.loop();
    websocket_fail_safe_check();
}

void websocket_fail_safe_check()
{
    if (!websocket_iniciado) return;
    if (webSocket.connectedClients() == 0) return;

    if (millis() - ultimo_comando_ms > WS_FAILSAFE_MS) {
        if (tipo_mov != 0) {
            tipo_mov = 0;
            rover_stop();
            Serial.println("Failsafe WebSocket: rover parado por timeout.");
        }
    }
}

void sendRadarData()
{
    if (enviar_dist != 1) return;
    if (webSocket.connectedClients() == 0) return;

    char buffer[JSON_BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer),
             "{\"type\":\"radar\",\"ang\":%d,\"dis\":%d}",
             ang_radar, dist_radar);

    webSocket.broadcastTXT(buffer);
    enviar_dist = 0;
}

void task_websockets(void *pvParameters)
{
    (void)pvParameters;

    init_websocket();
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        webSocket.loop();
        websocket_fail_safe_check();

        if (enabled_task_websockets == 1 && webSocket.connectedClients() > 0) {
            char buffer[JSON_BUFFER_SIZE];

            snprintf(buffer, sizeof(buffer),
                     "{\"type\":\"sensor\",\"t\":%.2f,\"h\":%.2f}",
                     valores_DHT11.temperature, valores_DHT11.humidity);
            webSocket.broadcastTXT(buffer);

            snprintf(buffer, sizeof(buffer),
                     "{\"type\":\"giros\",\"a\":%.2f,\"e\":%.2f,\"g\":%.2f}",
                     angleZ, angleY, angleX);
            webSocket.broadcastTXT(buffer);

            if (enabled_task_radar == 1 && enviar_dist == 1) {
                snprintf(buffer, sizeof(buffer),
                         "{\"type\":\"radar\",\"ang\":%d,\"dis\":%d}",
                         ang_radar, dist_radar);
                webSocket.broadcastTXT(buffer);
                enviar_dist = 0;
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_websockets));
    }
}