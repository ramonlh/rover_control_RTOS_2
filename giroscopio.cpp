
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

void task_giroscopio(void *pvParameters)
{
  (void)pvParameters;

  esp_task_wdt_add(NULL);

  if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
    JY901.startIIC(i2caddress_giroscopio);
    xSemaphoreGive(i2cMutex);
  }

  // calibrarGiroscopio();

  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    esp_task_wdt_reset();

    {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
        esp_task_wdt_reset();
        angleX = JY901.getRoll();
        xSemaphoreGive(i2cMutex);
      }

      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
        esp_task_wdt_reset();
        angleY = JY901.getPitch();
        xSemaphoreGive(i2cMutex);
      }

      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
        esp_task_wdt_reset();
        angleZ = JY901.getYaw();
        xSemaphoreGive(i2cMutex);
      }

      if (angleZ < 0)
        angleZ = -angleZ;
      else
        angleZ = 360 - angleZ;
    }

    esp_task_wdt_reset();
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_giroscopio));
  }
}