
#ifndef GIROSCOPIO_H
#define GIROSCOPIO_H

#include <Arduino.h>

extern float angleX;
extern float angleY;
extern float angleZ;

void task_giroscopio(void *pvParameters);

#endif