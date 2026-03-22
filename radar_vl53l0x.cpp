
#include "defines.h"
#include "radar_vl53l0x.h"
#include "servomotores.h"

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <Adafruit_VL53L0X.h>

extern SemaphoreHandle_t i2cMutex;
Adafruit_VL53L0X lox;

extern SemaphoreHandle_t i2cMutex;

int ang_radar = 0;
int dist_radar = 0;
int enviar_dist = 0;
int enabled_radar = 0;
extern SemaphoreHandle_t i2cMutex;

void set_pos_radar(int angulo)
{
  angulo_servo_3 = angulo;
  if (enabled_radar == 1)
    {
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
    if (measure.RangeStatus != 4) {  // phase failures have incorrect data
      dist_radar=measure.RangeMilliMeter;    // en mm
      //Serial.println(dist_radar);
      } 
    else {
      dist_radar = 0;
      //Serial.println("ERROR Radar");
      }
    }
  else
    {
    dist_radar=0;
    }
  if (dist_radar > 1500)
    dist_radar = 0;
  radar_leido = 1;
  enviar_dist = 1;
}

void task_radar(void *pvParameters) 
{  
  esp_task_wdt_add(NULL);

  if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
  if (!lox.begin()) {
    Serial.println("VL53L0X detectado: NO");
    enabled_radar = 0;
    } 
  else {
    Serial.println("VL53L0X detectado: OK");
    enabled_radar = 1;
    }     
    xSemaphoreGive(i2cMutex);
  }

  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    esp_task_wdt_reset();
    //Serial.printf("enabled_task_radar=%d enabled_radar=%d ang_radar=%d\n",
    //          enabled_task_radar, enabled_radar, ang_radar);
    if (enabled_task_radar == 1 && enabled_radar == 1) {

      for (int i = 0; i <= 178; i += 2) {
        esp_task_wdt_reset();
        ang_radar = i;

        int posservo = (ang_radar * (ang_lim[SERVO3_CHANNEL][1] - ang_lim[SERVO3_CHANNEL][0]) / 180) + 10;

        if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50))) {
          set_pos_radar(posservo);
          xSemaphoreGive(i2cMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(200));
      }

      for (int i = 178; i >= 0; i -= 2) {
        esp_task_wdt_reset();
        ang_radar = i;

        int posservo = (ang_radar * (ang_lim[SERVO3_CHANNEL][1] - ang_lim[SERVO3_CHANNEL][0]) / 180) + 10;

        if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50))) {
          set_pos_radar(posservo);
          xSemaphoreGive(i2cMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(200));
      }
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_radar));
  }
}