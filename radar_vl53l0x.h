
#ifndef RADAR_VL53L0X_H
#define RADAR_VL53L0X_H

#include <Arduino.h>

extern int ang_radar;
extern int dist_radar;
extern int enviar_dist;

void set_pos_radar(int angulo);
void task_radar(void *pvParameters);

#endif