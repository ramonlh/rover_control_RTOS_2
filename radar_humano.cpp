
#include "defines.h"
#include "radar_humano.h"

RCWL0516 radarhumano(pin_radar);

volatile int presencia_humana = 0;
volatile int enviar_presencia = 0;

namespace {
    bool flash_presencia_activo = false;
    uint32_t flash_presencia_hasta = 0;
}

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
        //Serial.println(estado_actual);
        if (estado_actual != ultimo_estado) {
            ultimo_estado = estado_actual;
            presencia_humana = estado_actual;
            enviar_presencia = 1;

            // Solo al detectar presencia
            if (presencia_humana == 1) {
                digitalWrite(pin_led_7colores, HIGH);
                //flash_presencia_activo = true;
                //flash_presencia_hasta = millis() + 3000;
            }
        }

        // Apagado automático del flash
        if (flash_presencia_activo) {
            if ((int32_t)(millis() - flash_presencia_hasta) >= 0) {
                digitalWrite(pin_led_7colores, LOW);
                flash_presencia_activo = false;
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_radarhumano));
    }
}