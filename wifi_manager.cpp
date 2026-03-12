
#include "defines.h"
#include "wifi_manager.h"

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

extern int modo_conex;

bool wm_nonblocking = false;
WiFiManager wm;
WiFiManagerParameter custom_field("");

String getParam(String name) {
  if (wm.server->hasArg(name)) {
    return wm.server->arg(name);
  }
  return "";
}

void saveParamCallback() {
  Serial.println(F("[CALLBACK] saveParamCallback Ok"));
  Serial.print(F("PARAM customfieldid = "));
  Serial.println(getParam("customfieldid"));
}

void checkButton() {
  if (digitalRead(WIFI_TRIGGER_PIN) == LOW) {
    delay(50);
    if (digitalRead(WIFI_TRIGGER_PIN) == LOW) {
      #ifdef DEBUG
        Serial.println(F("Button Pressed"));
      #endif

      delay(3000);

      if (digitalRead(WIFI_TRIGGER_PIN) == LOW) {
        #ifdef DEBUG
          Serial.println(F("Boton pres.. Borrando conf. reinit"));
        #endif
        wm.resetSettings();
        ESP.restart();
      }

      #ifdef DEBUG
        Serial.println(F("Init portal conf"));
      #endif

      wm.setConfigPortalTimeout(120);

      if (!wm.startConfigPortal("OnDemandAP", "password")) {
        #ifdef DEBUG
          Serial.println(F("No se pudo conectar"));
        #endif
        delay(3000);
      } else {
        #ifdef DEBUG
          Serial.println(F("conectado OK"));
        #endif
      }
    }
  }
}

void init_wifi_manager()
{
  const char *ap_ssid = "ROVER_DIEGO";
  const char *ap_password = "12341234";

  if (modo_conex == 0) {
    WiFi.mode(WIFI_AP_STA);

    #ifdef DEBUG
      Serial.print(F("\n Iniciando WiFi: "));
      Serial.print(ap_ssid);
      Serial.print("/");
      Serial.println(ap_password);
    #endif

    pinMode(WIFI_TRIGGER_PIN, INPUT);

    const char* custom_radio_str =
      "<br/><label for='customfieldid'>Custom Field</label>"
      "<input type='radio' name='customfieldid' value='1' checked> One<br>"
      "<input type='radio' name='customfieldid' value='2'> Two<br>"
      "<input type='radio' name='customfieldid' value='3'> Three";

    new (&custom_field) WiFiManagerParameter(custom_radio_str);

    wm.addParameter(&custom_field);
    wm.setSaveParamsCallback(saveParamCallback);

    std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
    wm.setMenu(menu);
    wm.setClass("invert");

    WiFi.softAP(ap_ssid, ap_password);

    bool res = wm.autoConnect();

    #ifdef DEBUG
      if (!res) {
        Serial.println(F("No se pudo conectar"));
      } else {
        Serial.println(F("conectado OK"));
      }
    #endif
  }
  else if (modo_conex == 1) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);

    #ifdef DEBUG
      Serial.print(F("\n Iniciando WiFi: "));
      Serial.print(ap_ssid);
      Serial.print("/");
      Serial.println(ap_password);
    #endif
  }
  else if (modo_conex == 2) {
    WiFi.mode(WIFI_OFF);
  }
}

void wifimanager_loop()
{
  if (wm_nonblocking) {
    wm.process();
  }
  checkButton();
}