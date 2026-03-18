
#include "sistema_ficheros.h"

#include <FS.h>
#include <FFat.h>
#include <Arduino.h>

#include <FFat.h>

void listarFFat(fs::FS &fs, const char * dirname, uint8_t levels)
{
  Serial.print("Listando: ");
  Serial.println(dirname);

  File root = fs.open(dirname);
  if (!root || !root.isDirectory()) {
    Serial.println("No es directorio");
    return;
  }

  File file = root.openNextFile();
  while (file) {  
    Serial.print("  ");
    Serial.print(file.name());
    Serial.print("  ");
    Serial.println(file.size());
    file = root.openNextFile();
  }
}

void ini_sistema_ficheros()
{
  if (!FFat.begin(true)) {
    #ifdef DEBUG
      Serial.println(F("Error FFat"));
    #endif
    return;
  }

  #ifdef DEBUG
    Serial.println(F("FFat OK"));
    listarFFat(FFat, "/", 0);    
  #endif
}