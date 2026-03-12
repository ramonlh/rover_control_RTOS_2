
#include "defines.h"
#include "ota.h"

#include <Arduino.h>
#include <ArduinoOTA.h>

void init_ota()
{
  ArduinoOTA.setPassword(password_OTA);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {
        type = "filesystem";
      }

      Serial.print(F("Start updating "));
      Serial.println(type);
    })
    .onEnd([]() {
      Serial.println(F("\nEnd"));
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println(F("Auth"));
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println(F("Begin"));
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println(F("Connect"));
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println(F("Receive"));
      } else if (error == OTA_END_ERROR) {
        Serial.println(F("End"));
      }
    });

  ArduinoOTA.begin();
}