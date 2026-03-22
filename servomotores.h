
#ifndef SERVOMOTORES_H
#define SERVOMOTORES_H

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

// Rango de pulsos del servo
#define SERVO_MIN  110
#define SERVO_MAX  490

// Canales del PCA9685 para los servos
#define SERVO1_CHANNEL 8
#define SERVO2_CHANNEL 9
#define SERVO3_CHANNEL 13

extern Adafruit_PWMServoDriver servos;

extern int radar_leido;
extern int angulo_servo_1;
extern int angulo_servo_2;
extern int angulo_servo_3;
extern int ang_lim[16][2];

void moverServo(uint8_t canal, int angulo);
void task_servomotores(void *pvParameters);

#endif