

#include <FS.h>
#include <SPIFFS.h>  // SPIFFS es el sistema de archivos por defect

void ini_sistema_ficheros()
{
  // Inicializar SPIFFS
  if (!SPIFFS.begin(true)) {  // El parámetro 'true' formatea el sistema de archivos si no está montado
    #ifdef DEBUG
      Serial.println(F("Error SPIFFS"));
    #endif
    return;
  }
  #ifdef DEBUG
    Serial.println(F("SPIFFS OK"));
  #endif
  // Verificar el espacio disponible
}