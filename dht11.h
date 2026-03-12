
#ifndef DHT11_TASK_H
#define DHT11_TASK_H

#include <Arduino.h>
#include <DHTesp.h>

extern DHTesp dht11;
extern TempAndHumidity valores_DHT11;

void task_dht11(void *pvParameters);

#endif