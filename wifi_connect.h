

#include "WiFi.h"

const char *ssid = "MikroTik-E1E1FB";
const char *password = "18921892";  

const char *ssidAP = "ROVER_DIEGO";
const char *passwordAP = "12341234";  

void init_wifi()
{
  if (modo_conex==0)
    {
      // modo STA
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      }
    Serial.println("");
    Serial.print("WiFi conectado");     Serial.print("IP : ");
    Serial.println(WiFi.localIP());
    // Configura el servidor DNS después de la conexión. Necesario si se usa la librería WiFi_Manager
    IPAddress dns(8, 8, 8, 8);  // DNS de Google (puedes usar otro)
    IPAddress subnet(255, 255, 255, 0);  // DNS de Google (puedes usar otro)
    // modo AP
    WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), subnet, dns);  // Establece subnet y DNS  
    if (!WiFi.softAP(ssidAP, passwordAP)) {
      Serial.println(F("Soft AP Error."));
      while (1);
    }
    IPAddress myIP = WiFi.softAPIP();
    Serial.print(F("AP IP: "));  Serial.println(myIP);
    }
  else if (modo_conex==1)
    {
    // modo AP
    if (!WiFi.softAP(ssidAP, passwordAP)) {
      Serial.println(F("Soft AP Error."));
      while (1);
    }
    IPAddress myIP = WiFi.softAPIP();
    Serial.print(F("AP IP: "));  Serial.println(myIP);
    }
}

