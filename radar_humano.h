
#ifndef RADAR_HUMANO_H
#define RADAR_HUMANO_H

#include <Arduino.h>
#include "RCWL0516.h"

extern RCWL0516 radarhumano;

void task_radarhumano(void *pvParameters);

#endif