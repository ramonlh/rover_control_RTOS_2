
#include "defines.h"
#include "radar_humano.h"

RCWL0516 radarhumano(pin_radar);

void task_radarhumano(void *pvParameters)
{
  (void)pvParameters;

  radarhumano.activate();
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    if (enabled_task_radarhumano == 1) {
      int value = digitalRead(pin_radar);
      (void)value;
    }

    delay(1);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_radar));
  }
}