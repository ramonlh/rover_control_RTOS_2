
#include "defines.h"
#include "servidor_web.h"

#include <UnixTime.h>
#include <web_pages.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

WebServer webserver(PORT_WEBSERVER);
char other_ip[16];

static const char* textplain = "text/plain";

extern int enviar_dist;
extern int modo_conex;
extern String ssid;
extern String password;
extern const char* ssidAP;
extern String passwordAP;

String SendHTML()
{
  String ptr = "";
  ptr += head_1;
  ptr += String(other_ip);
  ptr += "192.168.4.2";
  ptr += head_2;
  ptr += script_01;
  return ptr;
}

String SendSetupHTML()
{
  enviar_dist = 0;
  String ptr = "";
  ptr += setup_page_1;
  ptr += "<div>AP ssid: <b>" + String(ssidAP) + "</b> AP ssid: <b>" + passwordAP + "</b></div>";
  ptr += setup_page_2;
  return ptr;
}

void send_200()
{
  webserver.send(200, F("text/html"), SendHTML());
}

void send_204()
{
  webserver.send(204);
}

void handle_index()
{
  send_200();
}

void handle_NotFound()
{
  webserver.send(404, textplain, F("La pagina no existe"));
}

void handle_image(const char* filename)
{
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    webserver.send(404, textplain, F("Archivo no encontrado"));
    return;
  }
  webserver.streamFile(file, "image/jpeg");
  file.close();
}

void handle_cochearriba() { handle_image("/carup.jpg"); }
void handle_cochetrasera() { handle_image("/carback.jpg"); }
void handle_cochelado() { handle_image("/carside.jpg"); }

void saveConfig(String& modoinic, String& ssid, String& password)
{
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("No se pudo abrir el archivo para guardar");
    return;
  }

  DynamicJsonDocument saveDoc(1024);
  saveDoc["modoinic"] = modoinic;
  saveDoc["ssid"] = ssid;
  saveDoc["password"] = password;

  serializeJson(saveDoc, configFile);
  configFile.close();
  Serial.println("Configuración guardada correctamente");
}

void readConfig()
{
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("No se pudo abrir el archivo para leer");
    return;
  }

  String config = configFile.readString();
  configFile.close();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, config);
  if (error) {
    Serial.println("Error al leer el archivo JSON");
    return;
  }

  modo_conex = doc["modoinic"].as<int>();
  ssid = doc["ssid"].as<String>();
  password = doc["password"].as<String>();
}

void handle_GetConfig()
{
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    webserver.send(500, "application/json", "{\"error\":\"No se pudo leer la configuración\"}");
    return;
  }

  String config = configFile.readString();
  configFile.close();
  webserver.send(200, "application/json", config);
}

void handle_setup()
{
  readConfig();
  webserver.send(200, F("text/html"), SendSetupHTML());
}

void handle_save_config()
{
  if (webserver.hasArg("plain") == false) {
    webserver.send(400, "application/json", "{\"error\":\"Datos no encontrados\"}");
    return;
  }

  String payload = webserver.arg("plain");
  Serial.println("guardar:");
  Serial.println(payload);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    webserver.send(400, "application/json", "{\"error\":\"Error al parsear JSON\"}");
    return;
  }

  String modoinic = doc["modoinic"];
  String ssid = doc["ssid"];
  String password = doc["password"];

  saveConfig(modoinic, ssid, password);
  webserver.send(200, F("text/html"), SendSetupHTML());
}

void init_webserver()
{
  webserver.on("/", handle_index);
  webserver.on("/carup", handle_cochearriba);
  webserver.on("/carside", handle_cochelado);
  webserver.on("/carback", handle_cochetrasera);
  webserver.on("/setup", handle_setup);
  webserver.on("/save_config", handle_save_config);
  webserver.on("/get_config", handle_GetConfig);

  webserver.onNotFound(handle_NotFound);
  webserver.begin();
}