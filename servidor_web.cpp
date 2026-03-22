
#include "servidor_web.h"


WebServer webserver(PORT_WEBSERVER);
char other_ip[16] = "192.168.4.2";


static const char* textplain = "text/plain";
static const char* CONFIG_FILE = "/config.json";

extern int enviar_dist;
extern int modo_conex;

extern const char* ssidAP;


namespace {

bool isValidModoConex(int modo)
{
  return (modo == 0 || modo == 1 || modo == 2);
}

int normalizeModoConex(int modo)
{
  return isValidModoConex(modo) ? modo : 0;
}

void loadDefaultConfig()
{
  modo_conex = normalizeModoConex(modo_conex);

if (ssid == nullptr || ssid[0] == '\0') {
    password = "";
  }
}

bool saveConfigToFFat(int modoinic, const String& newSsid, const String& newPassword)
{
  File configFile = FFat.open(CONFIG_FILE, FILE_WRITE);
  if (!configFile) {
    Serial.println("No se pudo abrir el archivo para guardar");
    return false;
  }

  DynamicJsonDocument saveDoc(512);
  saveDoc["modoinic"] = normalizeModoConex(modoinic);
  saveDoc["ssid"] = newSsid;
  saveDoc["password"] = newPassword;

  const size_t written = serializeJson(saveDoc, configFile);
  configFile.close();

  if (written == 0) {
    Serial.println("Error al guardar JSON");
    return false;
  }

  Serial.println("Configuración guardada correctamente");
  return true;
}

bool readConfigFromFFat()
{
  if (!FFat.exists(CONFIG_FILE)) {
    Serial.println("No existe config.json, usando configuración actual");
    loadDefaultConfig();
    return false;
  }

  File configFile = FFat.open(CONFIG_FILE, FILE_READ);
  if (!configFile) {
    Serial.println("No se pudo abrir el archivo para leer");
    loadDefaultConfig();
    return false;
  }

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();

  if (error) {
    Serial.println("Error al leer el archivo JSON");
    loadDefaultConfig();
    return false;
  }

  modo_conex = normalizeModoConex(doc["modoinic"] | modo_conex);
  strlcpy(ssid_buf, (const char*)(doc["ssid"] | ""), sizeof(ssid_buf));
  strlcpy(password_buf, (const char*)(doc["password"] | ""), sizeof(password_buf));

  return true;
}

} // namespace

String SendHTML()
{
  String ipCam = (other_ip[0] != '\0') ? String(other_ip) : "192.168.4.2";
  (void)ipCam;

  String ptr;
  ptr.reserve(4096);
  ptr += head_1;
  ptr += head_2;
  ptr += script_01;
  return ptr;
}

String SendSetupHTML()
{
  enviar_dist = 0;
  String ptr;
  ptr.reserve(3072);
  ptr += setup_page_1;
  ptr += "<div>AP ssid: <b>" + String(ssidAP) + "</b></div>";
  ptr += "<div>AP password: <b>********</b></div>";
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
  File file = FFat.open(filename, FILE_READ);
  if (!file) {
    Serial.print("No encontrada: ");
    Serial.println(filename);
    webserver.send(404, textplain, F("Archivo no encontrado"));
    return;
  }
  webserver.streamFile(file, "image/jpeg");
  file.close();
}

void handle_cochearriba() { handle_image("/carup.jpg"); }
void handle_cochetrasera() { handle_image("/carback.jpg"); }
void handle_cochelado() { handle_image("/carside.jpg"); }

void handle_GetConfig()
{
  DynamicJsonDocument doc(256);
  doc["modoinic"] = normalizeModoConex(modo_conex);
  doc["ssid"] = ssid;
  doc["password"] = "";

  String out;
  serializeJson(doc, out);
  webserver.send(200, "application/json", out);
}

void handle_setup()
{
  readConfigFromFFat();
  webserver.send(200, F("text/html"), SendSetupHTML());
}

void handle_save_config()
{
  if (!webserver.hasArg("plain")) {
    webserver.send(400, "application/json", "{\"ok\":false,\"error\":\"Datos no encontrados\"}");
    return;
  }

  const String payload = webserver.arg("plain");
  Serial.println("guardar:");
  Serial.println(payload);

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    webserver.send(400, "application/json", "{\"ok\":false,\"error\":\"Error al parsear JSON\"}");
    return;
  }

  int newModoInic = doc["modoinic"].is<int>()
                      ? doc["modoinic"].as<int>()
                      : String((const char*)(doc["modoinic"] | "")).toInt();

  if (!isValidModoConex(newModoInic)) {
    webserver.send(400, "application/json", "{\"ok\":false,\"error\":\"Modo de inicio no válido\"}");
    return;
  }

  String newSsid = String((const char*)(doc["ssid"] | ""));
  String newPassword = String((const char*)(doc["password"] | ""));
  newSsid.trim();

  if ((newModoInic == 0 || newModoInic == 2) && newSsid.isEmpty()) {
    webserver.send(400, "application/json", "{\"ok\":false,\"error\":\"Falta el SSID para modo cliente\"}");
    return;
  }

  if (!saveConfigToFFat(newModoInic, newSsid, newPassword)) {
    webserver.send(500, "application/json", "{\"ok\":false,\"error\":\"No se pudo guardar en FFat\"}");
    return;
  }

  modo_conex = newModoInic;
  strlcpy(ssid_buf, newSsid.c_str(), sizeof(ssid_buf));
  strlcpy(password_buf, newPassword.c_str(), sizeof(password_buf));

  webserver.send(200, "application/json", "{\"ok\":true}");
}






void init_webserver()
{
  readConfigFromFFat();

  webserver.on("/", HTTP_GET, handle_index);
  webserver.on("/carup", HTTP_GET, handle_cochearriba);
  webserver.on("/carside", HTTP_GET, handle_cochelado);
  webserver.on("/carback", HTTP_GET, handle_cochetrasera);
  webserver.on("/setup", HTTP_GET, handle_setup);
  webserver.on("/save_config", HTTP_POST, handle_save_config);
  webserver.on("/get_config", HTTP_GET, handle_GetConfig);
  webserver.on("/restart", HTTP_POST, []() { 
    webserver.send(200, "text/plain", "Reiniciando"); delay(300); ESP.restart();
    });
  webserver.onNotFound(handle_NotFound);
  webserver.begin();
}