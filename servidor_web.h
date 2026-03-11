
#include <UnixTime.h>
#include <web_pages.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WebServer webserver(PORT_WEBSERVER);    // #define port_servidor_web 91

char other_ip[16]; // 

//WiFiClient espClient;

String SendHTML() {
    // Cabecera de todas las paginas WEB
  String ptr = "";
  ptr += head_1;
  ptr += String(other_ip);
  ptr += "192.168.4.2";
  ptr += head_2;
  ptr += script_01;
  return ptr;
}

String SendSetupHTML() {
    // Cabecera de todas las paginas WEB
  enviar_dist=0;
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

void handle_index() {
  send_200();
}

const char* textplain = "text/plain";

void handle_NotFound() {
  webserver.send(404, textplain, F("La pagina no existe"));
}

void handle_image(const char* filename) {
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

void saveConfig(String& modoinic, String& ssid, String& password) {
  // Abre el archivo en modo escritura
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("No se pudo abrir el archivo para guardar");
    return;
    }
  // Crear un objeto JSON para guardar los datos
  DynamicJsonDocument saveDoc(1024);
  saveDoc["modoinic"] = modoinic;
  saveDoc["ssid"] = ssid;
  saveDoc["password"] = password;
  // Serializa el JSON y lo guarda en el archivo
  serializeJson(saveDoc, configFile);
  configFile.close();  // Cierra el archivo después de guardar
  Serial.println("Configuración guardada correctamente");
}

void readConfig() {
  // Abre el archivo en modo lectura
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("No se pudo abrir el archivo para leer");
    return;
    }
  // Lee el contenido del archivo como una cadena
  String config = configFile.readString();
  configFile.close();  // Cierra el archivo después de leer
  // Deserializa el JSON
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, config);
  if (error) {
    Serial.println("Error al leer el archivo JSON");
    return;
    }
  // Accede a los valores del JSON
  modo_conex = doc["modoinic"];
  ssid = doc["ssid"];
  password = doc["password"];
}

void handle_GetConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    webserver.send(500, "application/json", "{\"error\":\"No se pudo leer la configuración\"}");
    return;
    }
  String config = configFile.readString();
  configFile.close();
  webserver.send(200, "application/json", config);
}

void handle_setup() { 
  readConfig();
  webserver.send(200, F("text/html"), SendSetupHTML()); 
  }

void handle_save_config() { 
  if (webserver.hasArg("plain") == false) {  // Comprobar si se recibe un cuerpo JSON
    webserver.send(400, "application/json", "{\"error\":\"Datos no encontrados\"}");
    return;
    }
  String payload = webserver.arg("plain");
  Serial.println("guardar:");Serial.println(payload);
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    webserver.send(400, "application/json", "{\"error\":\"Error al parsear JSON\"}");
    return;
    }
  String modoinic = doc["modoinic"];
  String ssid = doc["ssid"];
  String password = doc["password"];
  saveConfig(modoinic, ssid, password);   // Guardar los datos en SPIFSS
  webserver.send(200, F("text/html"), SendSetupHTML()); 
  }

void init_webserver()
{
  webserver.on("/", handle_index); 
  // carga imagenes
  webserver.on("/carup", handle_cochearriba);
  webserver.on("/carside", handle_cochelado);
  webserver.on("/carback", handle_cochetrasera);
  webserver.on("/setup", handle_setup);
  webserver.on("/save_config", handle_save_config);
  webserver.on("/get_config", handle_GetConfig);
  
  webserver.onNotFound(handle_NotFound); 
  webserver.begin();
}

