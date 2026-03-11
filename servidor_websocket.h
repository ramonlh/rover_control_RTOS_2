
#include <ArduinoJson.h>

WebSocketsServer webSocket = WebSocketsServer(PORT_WEBSOCKET); // #define port_websockets 92

// Tamaño del buffer para almacenar el JSON
#define JSON_BUFFER_SIZE 128
char jsonBuffer[JSON_BUFFER_SIZE]; // Buffer estático para JSON

void scanWiFiNetworks() {
    int n = WiFi.scanNetworks();
    String ssidList = "["; // Formato JSON
    for (int i = 0; i < n; i++) {
        if (i > 0) ssidList += ",";
        ssidList += "\"" + WiFi.SSID(i) + "\"";
    }
    ssidList += "]";
    Serial.println("Enviando SSIDs: " + ssidList);
    webSocket.broadcastTXT(ssidList);  // Envía la lista a todos los clientes conectados
}

// Manejar mensajes de WebSocket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) {
    // Usar un buffer de tamaño fijo para manejar el mensaje
    char message[length + 1]; // Crear un buffer temporal
    memcpy(message, payload, length);
    message[length] = '\0'; // Asegurarse de que la cadena termine en '\0'
    Serial.println(message);
    // Intentar parsear el mensaje como JSON
    StaticJsonDocument<200> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, message);
    if (!error) {
      // Extraer valores del JSON para las direcciones (d1, d2, d3, d4) y velocidades (s1, s2, s3, s4)
      int d_trd = jsonDoc["d1"]; // Trasera Derecha (Rueda 1)
      int d_tri = jsonDoc["d2"]; // Trasera Izquierda (Rueda 2)
      int d_ded = jsonDoc["d3"]; // Delantera Derecha (Rueda 3)
      int d_dei = jsonDoc["d4"]; // Delantera Izquierda (Rueda 4)
      int s_trd = jsonDoc["s1"]; // Velocidad de la Trasera Derecha
      int s_tri = jsonDoc["s2"]; // Velocidad de la Trasera Izquierda
      int s_ded = jsonDoc["s3"]; // Velocidad de la Delantera Derecha
      int s_dei = jsonDoc["s4"]; // Velocidad de la Delantera Izquierda
      // Llamar a la función rover_move con los valores extraídos
      rover_move(d_trd, d_tri, d_ded, d_dei, s_trd, s_tri, s_ded, s_dei);
      } 
    else 
      {
      if      (strcmp(message, "f") == 0) { tipo_mov = 1; }
      else if (strcmp(message, "r") == 0) { tipo_mov = 2; }
      else if (strcmp(message, "fl") == 0) { tipo_mov = 3; }
      else if (strcmp(message, "fr") == 0) { tipo_mov = 4; }
      else if (strcmp(message, "rl") == 0) { tipo_mov = 5; }
      else if (strcmp(message, "rr") == 0) { tipo_mov = 6; }
      else if (strcmp(message, "rotl") == 0) { tipo_mov = 7; }
      else if (strcmp(message, "rotr") == 0) { tipo_mov = 8; }
      else if (strcmp(message, "latl") == 0) { tipo_mov = 9; }
      else if (strcmp(message, "latr") == 0) { tipo_mov = 10; }
      else if (strcmp(message, "stop") == 0) { tipo_mov = 0; }
      else if (strcmp(message, "caml") == 0) { tipo_mov = 31; angulo_servo_1 += 1; }
      else if (strcmp(message, "camr") == 0) { tipo_mov = 32; angulo_servo_1 -= 1; }
      else if (strcmp(message, "camup") == 0) { tipo_mov = 33; angulo_servo_2 -= 1; }
      else if (strcmp(message, "camdn") == 0) { tipo_mov = 34; angulo_servo_2 += 1; }
      else if (strcmp(message, "DIF") == 0) { tipo_mov = 41; }
      else if (strcmp(message, "DDF") == 0) { tipo_mov = 42; }
      else if (strcmp(message, "TIF") == 0) { tipo_mov = 43; }
      else if (strcmp(message, "TDF") == 0) { tipo_mov = 44; }
      else if (strcmp(message, "DIR") == 0) { tipo_mov = 45; }
      else if (strcmp(message, "DDR") == 0) { tipo_mov = 46; }
      else if (strcmp(message, "TIR") == 0) { tipo_mov = 47; }
      else if (strcmp(message, "TDR") == 0) { tipo_mov = 48; }
      
      else if (strcmp(message, "speedmenos") == 0) {  // reducir la velocidad
        if (rover_speed > 500) rover_speed -= 500;
        if (rover_speed < 500) rover_speed = 500;
        Serial.println(rover_speed);
        }
      else if (strcmp(message, "speedmas") == 0) {  // aumentar la velocidad
        if (rover_speed < 4000) rover_speed += 500;
        if (rover_speed > 4000) rover_speed = 4000;
        Serial.println(rover_speed);
        }
      else if (strcmp(message, "flash") == 0) {
        tipo_mov = 99;
        digitalWrite(pin_led_7colores, !digitalRead(pin_led_7colores)); // Alternar estado de las luces
        } 
      else if (strcmp(message, "luces") == 0) {
        tipo_mov = 99;
        digitalWrite(pin_led_izquierda, !digitalRead(pin_led_izquierda)); // Alternar estado de las luces
        digitalWrite(pin_led_derecha, !digitalRead(pin_led_derecha)); // Alternar estado de las luces
        } 
      else if (strcmp(message, "getssids") == 0) {
        tipo_mov = 99;
        scanWiFiNetworks();
        } 
      }

    }
}

void sendRadarData() {
  if (enviar_dist == 1)
    {
    if (webSocket.connectedClients() > 0) { // Solo enviar si hay clientes conectados
      snprintf(jsonBuffer, JSON_BUFFER_SIZE, "{\"type\":\"radar\", \"ang\":%ld, \"dis\":%ld}", ang_radar, dist_radar);
      webSocket.broadcastTXT(jsonBuffer);
      enviar_dist = 0;
      }
    }
}

void task_websockets(void *pvParameters) {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(1) {
    // send sensor data
    if (enabled_task_websockets == 1) {
      if (webSocket.connectedClients() > 0) { // Solo enviar si hay clientes conectados
        snprintf(jsonBuffer, JSON_BUFFER_SIZE, "{\"type\":\"sensor\", \"t\":%.2f, \"h\":%.2f}", valores_DHT11.temperature, valores_DHT11.humidity);
        webSocket.broadcastTXT(jsonBuffer);
        }
    // send giros data
      if (webSocket.connectedClients() > 0) { // Solo enviar si hay clientes conectados
        snprintf(jsonBuffer,JSON_BUFFER_SIZE,"{\"type\":\"giros\",\"a\":%.2f,\"e\":%.2f,\"g\":%.2f}",angleZ, angleY, angleX);
        webSocket.broadcastTXT(jsonBuffer);
        }
    // send radar data
      if (enabled_radar == 1) { 
        if (enviar_dist == 1)
          {
          if (webSocket.connectedClients() > 0) { // Solo enviar si hay clientes conectados
            snprintf(jsonBuffer, JSON_BUFFER_SIZE, "{\"type\":\"radar\", \"ang\":%ld, \"dis\":%ld}", ang_radar, dist_radar);
            webSocket.broadcastTXT(jsonBuffer);
            enviar_dist = 0;
            }
          }
        }
      }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_websockets));
    }
}

