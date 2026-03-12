
#include "sistema_ficheros.h"

#include <FS.h>
#include <SPIFFS.h>
#include <Arduino.h>

void ini_sistema_ficheros()
{
  if (!SPIFFS.begin(true)) {
    #ifdef DEBUG
      Serial.println(F("Error SPIFFS"));
    #endif
    return;
  }

  #ifdef DEBUG
    Serial.println(F("SPIFFS OK"));
  #endif
}