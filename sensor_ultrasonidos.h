
#ifndef SENSOR_ULTRASONIDOS_H
#define SENSOR_ULTRASONIDOS_H

#include <Arduino.h>

extern const int US1_EchoPin;
extern const int US1_TriggerPin;
extern int enabled_ultrasound;
extern unsigned int dUS1;

int ping(int TriggerPin, int EchoPin);
void task_ultrasonidos(void *pvParameters);

#endif