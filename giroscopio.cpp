
#include "defines.h"
#include "giroscopio.h"

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <JY901.h>

extern SemaphoreHandle_t i2cMutex;

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;

static float offsetX = 0.0f;
static float offsetY = 0.0f;
static float offsetZ = 0.0f;
static float rumbo_objetivo = 0.0f;

// Función para calibrar el giroscopio
static void calibrarGiroscopio()
{
  int num_muestras = 100;
  float sumX = 0.0f, sumY = 0.0f, sumZ = 0.0f;

  for (int i = 0; i < num_muestras; i++) {
    sumX += JY901.getRoll();
    sumY += JY901.getPitch();
    sumZ += JY901.getYaw();
    vTaskDelay(pdMS_TO_TICKS(20));
  }

  offsetX = sumX / num_muestras;
  offsetY = sumY / num_muestras;
  offsetZ = sumZ / num_muestras;
  (void)rumbo_objetivo;
}

volatile bool giroscopio_ok = false;

void task_giroscopio(void *pvParameters)
{
  (void)pvParameters;

  esp_task_wdt_add(NULL);

  angleX = 0.0f;
  angleY = 0.0f;
  angleZ = 0.0f;
  giroscopio_ok = false;

  const float offset_azimut = 180.0f;

  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    esp_task_wdt_reset();

    // Intentar iniciar el giroscopio hasta conseguirlo
    if (!giroscopio_ok) {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        JY901.startIIC(i2caddress_giroscopio);
        xSemaphoreGive(i2cMutex);

        vTaskDelay(pdMS_TO_TICKS(100));
        giroscopio_ok = true;
        Serial.println("JY901 iniciado");
      } else {
        static uint32_t ultimoAvisoInit = 0;
        uint32_t ahora = millis();
        if ((uint32_t)(ahora - ultimoAvisoInit) > 1000) {
          Serial.println("JY901: esperando i2cMutex para iniciar");
          ultimoAvisoInit = ahora;
        }
      }

      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }

    // Lectura normal
    if (giroscopio_ok) {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        float roll  = JY901.getRoll();
        float pitch = JY901.getPitch();
        float yaw   = JY901.getYaw();
        xSemaphoreGive(i2cMutex);
        
if (isfinite(roll) && isfinite(pitch) && isfinite(yaw)) {
  angleX = roll;
  angleY = pitch;

  if (yaw < 0.0f) yaw += 360.0f;

  yaw = yaw - offset_azimut;
  if (yaw < 0.0f) yaw += 360.0f;
  if (yaw >= 360.0f) yaw -= 360.0f;

  yaw = 360.0f - yaw;
  if (yaw >= 360.0f) yaw -= 360.0f;

  angleZ = yaw;
}
      } else {
        static uint32_t ultimoAviso = 0;
        static uint32_t numTimeouts = 0;

        numTimeouts++;
        uint32_t ahora = millis();

        if ((uint32_t)(ahora - ultimoAviso) > 1000) {
          if (numTimeouts > 3) {
            Serial.print("JY901: timeout i2cMutex x");
            Serial.println(numTimeouts);
          }
          numTimeouts = 0;
          ultimoAviso = ahora;
        }
      }
    }

    esp_task_wdt_reset();
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_giroscopio));
  }
}