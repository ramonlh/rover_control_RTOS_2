
#include "defines.h"
#include "radar_humano.h"
#include "flash_manager.h"

RCWL0516 radarhumano(pin_radar);

volatile int presencia_humana = 0;
volatile int enviar_presencia = 0;

void init_radarhumano()
{
  pinMode(pin_radar, INPUT);
}

void task_radarhumano(void *pvParameters)
{
    (void)pvParameters;
    init_radarhumano();

    int ultimo_estado = -1;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        int value = digitalRead(pin_radar);
        int estado_actual = value ? 1 : 0;

        if (estado_actual != ultimo_estado) {
            ultimo_estado = estado_actual;
            presencia_humana = estado_actual;
            enviar_presencia = 1;

            // Solo al detectar flanco de subida
            if (presencia_humana == 1) {
                flash_pulse(3000);
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_radarhumano));
    }
}