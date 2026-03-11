

#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

int ang_radar = 0;
int dist_radar = 0;
int enviar_dist = 0;
int enabled_radar = 0;

void set_pos_radar(int angulo)
{
  angulo_servo_3 = angulo;
  if (enabled_radar == 1)
    {
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
    if (measure.RangeStatus != 4) {  // phase failures have incorrect data
      dist_radar=measure.RangeMilliMeter;    // en mm
      } 
    }
  else
    {
    dist_radar=0;
    }
  radar_leido = 1;
  enviar_dist = 1;
}

void task_radar(void *pvParameters) 
{  
  esp_task_wdt_add(NULL);
  if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
    if (!lox.begin()) {
      enabled_radar = 0;
      }
    else 
      { 
      enabled_radar = 1;
      }
    xSemaphoreGive(i2cMutex);
    }
  TickType_t xLastWakeTime = xTaskGetTickCount();
  int radar_leido=0;
  
  while (1) { // La tarea nunca debe salir
    esp_task_wdt_reset();
    if (enabled_task_radar == 1) {
      int i;
      for (i = 0; i<176; i++)
        {
        esp_task_wdt_reset();
        ang_radar = i;
        int posservo = (ang_radar * (ang_lim[SERVO3_CHANNEL][1]-ang_lim[SERVO3_CHANNEL][0]) / 180) + 10;
        if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
          set_pos_radar(posservo);
          xSemaphoreGive(i2cMutex);
          }
        }
      for (i = 176; i>0; i--)
        {
        esp_task_wdt_reset();
        ang_radar = i;
        int posservo = (ang_radar * (ang_lim[SERVO3_CHANNEL][1]-ang_lim[SERVO3_CHANNEL][0]) / 180) + 10;
        if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
          set_pos_radar(posservo);
          xSemaphoreGive(i2cMutex);
          }
        }
      }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_radar));
  }
}
