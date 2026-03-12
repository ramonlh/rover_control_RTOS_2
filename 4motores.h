
#ifndef CUATRO_MOTORES_H
#define CUATRO_MOTORES_H

#include <Arduino.h>

extern int rover_speed;
extern int tipo_mov;
extern volatile int control_activo;
extern const char* text_accion[10];

void digitalWriteMux(int pin, int value);
void set_motor(int motor, int dir, int speed);
void set_speed_rover(int velocidad);
void paratodo();
void rover_stop();
void rover_move(int dirDI, int dirDD, int dirTD, int dirTI,
                int speedDI, int speedDD, int speedTD, int speedTI);
void handle_obstaculos();
void task_motores(void *pvParameters);

#endif