
#include "defines.h"
#include "wifi_connect.h"

#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "MikroTik-E1E1FB";
const char *password = "18921892";

const char *ssidAP = "ROVER_DIEGO";
const char *passwordAP = "12341234";

extern int modo_conex;

void init_wifi()
{
  if (modo_conex == 0) {
    // modo STA
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.print("WiFi conectado");
    Serial.print("IP : ");
    Serial.println(WiFi.localIP());

    // Configura el servidor DNS después de la conexión
    IPAddress dns(8, 8, 8, 8);
    IPAddress subnet(255, 255, 255, 0);

    // modo AP
    WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), subnet, dns);
    if (!WiFi.softAP(ssidAP, passwordAP)) {
      Serial.println(F("Soft AP Error."));
      while (1) {}
    }

    IPAddress myIP = WiFi.softAPIP();
    Serial.print(F("AP IP: "));
    Serial.println(myIP);
  }
  else if (modo_conex == 1) {
    // modo AP
    if (!WiFi.softAP(ssidAP, passwordAP)) {
      Serial.println(F("Soft AP Error."));
      while (1) {}
    }

    IPAddress myIP = WiFi.softAPIP();
    Serial.print(F("AP IP: "));
    Serial.println(myIP);
  }
}