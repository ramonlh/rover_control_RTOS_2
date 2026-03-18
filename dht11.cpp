

#include "defines.h"
#include "dht11.h"

DHTesp dht11;
TempAndHumidity valores_DHT11 = {NAN, NAN};

void task_dht11(void *pvParameters)
{
    (void)pvParameters;

    dht11.setup(pin_DHT11, DHTesp::DHT11);
    vTaskDelay(pdMS_TO_TICKS(1500));

    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        if (enabled_task_DHT == 1) {
            TempAndHumidity newValues = dht11.getTempAndHumidity();

            if (!isnan(newValues.temperature) &&
                !isnan(newValues.humidity) &&
                dht11.getStatus() == 0) {
                valores_DHT11 = newValues;
            } else {
                static uint32_t ultimoAviso = 0;
                uint32_t ahora = millis();
                if ((uint32_t)(ahora - ultimoAviso) > 5000) {
                    Serial.print("DHT11 error: ");
                    Serial.println(dht11.getStatusString());
                    ultimoAviso = ahora;
                }
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_DHT11));
    }
}