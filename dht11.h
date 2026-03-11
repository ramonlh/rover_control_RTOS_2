
#include "DHTesp.h" 

//DHTesp dht;
/** Pin number for DHT11 data pin */
//#define pin_DHT11 13

DHTesp dht11;
TempAndHumidity valores_DHT11;

void task_dht11(void *pvParameters) {
    dht11.setup(pin_DHT11, DHTesp::DHT11);
    TempAndHumidity newValues;
    newValues = dht11.getTempAndHumidity();
      if (!isnan(newValues.temperature) && !isnan(newValues.humidity)) {
        valores_DHT11 = newValues; // Guardar valores válidos
      }
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1) {
      if (enabled_task_DHT == 1)
        {
        newValues = dht11.getTempAndHumidity();
        if (!isnan(newValues.temperature) && !isnan(newValues.humidity)) {
          valores_DHT11 = newValues; // Guardar valores válidos
          }
        }
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_DHT11)); // Leer cada 10s
      }
}
