
#include <JY901.h>

float angleX = 0.0, angleY = 0.0, angleZ = 0.0;
float offsetX = 0.0, offsetY = 0.0, offsetZ = 0.0;
float rumbo_objetivo;

// Función para calibrar el giroscopio
void calibrarGiroscopio() {

  int num_muestras = 100;
  float sumX = 0, sumY = 0, sumZ = 0;
  for (int i = 0; i < num_muestras; i++) {
    sumX += JY901.getRoll();
    sumY += JY901.getPitch();
    sumZ += JY901.getYaw();
    vTaskDelay(pdMS_TO_TICKS(20));  // Espera 20 ms entre muestras
  }
  // Calcular el offset promedio
  offsetX = sumX / num_muestras;
  offsetY = sumY / num_muestras;
  offsetZ = sumZ / num_muestras;
}

// Definir la función que se ejecutará en la tarea
void task_giroscopio(void *pvParameters) {
  esp_task_wdt_add(NULL);
  if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
    JY901.startIIC(i2caddress_giroscopio);
    xSemaphoreGive(i2cMutex);
    }
  // Realizar calibración antes de iniciar las lecturas
  //calibrarGiroscopio();
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(1) {
    esp_task_wdt_reset();
    //if (enabled_task_giroscopio == 1)
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
      if (angleZ < 0)      // esto llevarlo fuera
        angleZ = -angleZ;
      else
        angleZ = 360 - angleZ;
      }
    esp_task_wdt_reset();
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_giroscopio));
  }

}

