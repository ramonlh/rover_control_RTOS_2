

#include "RCWL0516.h"

RCWL0516 radarhumano(pin_radar);

// Definir la función que se ejecutará en la tarea
void task_radarhumano(void *pvParameters) {
  // Initialize sensor radar
  radarhumano.activate();
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(1) {
    if (enabled_task_radarhumano == 1) {
      int value = digitalRead(pin_radar);  
      }
    delay(1); 
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_radar));
  }
}
