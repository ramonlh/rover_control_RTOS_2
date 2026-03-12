
#include "defines.h"
#include "4motores.h"
#include "mux_mcp23017.h"
#include "mux_servos_pca9685.h"
#include "radar_vl53l0x.h"

#include <Arduino.h>
#include <esp_task_wdt.h>

#define MFORWARD 1
#define MBACK 0

extern SemaphoreHandle_t i2cMutex;

const uint8_t pin_pwm_motor[4] = {2, 3, 1, 0};
const uint8_t pin_dir[4][2] = {{0, 1}, {2, 3}, {4, 5}, {6, 7}};

int rover_speed = 1000;
int tipo_mov = 99;
volatile int control_activo = 0;
const char* text_accion[10] = {"f","r","fl","fr","rl","rr","rotl","rotr","latl","latr"};

void digitalWriteMux(int pin, int value)
{
  mcp.digitalWrite((pins)pin, value == 0 ? LOW : HIGH);
}

void set_motor(int motor, int dir, int speed)
{
  if (speed == 0) {
    digitalWriteMux(pin_dir[motor][0], HIGH);
    digitalWriteMux(pin_dir[motor][1], HIGH);
    speed_motor.setValue(pin_pwm_motor[motor], 0);
  } else {
    digitalWriteMux(pin_dir[motor][0], (dir == 0) ? LOW : HIGH);
    digitalWriteMux(pin_dir[motor][1], (dir == 0) ? HIGH : LOW);
    speed_motor.setValue(pin_pwm_motor[motor], speed);
  }
}

void set_speed_rover(int velocidad)
{
  rover_speed = constrain(velocidad, 500, 4000);
  for (int i = 0; i < 4; i++) {
    speed_motor.setValue(i, rover_speed);
  }
}

void paratodo()
{
  digitalWriteMux(pin_dir[0][0], HIGH);
  digitalWriteMux(pin_dir[0][1], HIGH);
  digitalWriteMux(pin_dir[1][0], HIGH);
  digitalWriteMux(pin_dir[1][1], HIGH);
  digitalWriteMux(pin_dir[2][0], HIGH);
  digitalWriteMux(pin_dir[2][1], HIGH);
  digitalWriteMux(pin_dir[3][0], HIGH);
  digitalWriteMux(pin_dir[3][1], HIGH);
  speed_motor.setValue(0, 0);
  speed_motor.setValue(1, 0);
  speed_motor.setValue(2, 0);
  speed_motor.setValue(3, 0);
}

void rover_stop()
{
  paratodo();
}

void rover_move(int dirDI, int dirDD, int dirTD, int dirTI,
                int speedDI, int speedDD, int speedTD, int speedTI)
{
  set_motor(0, dirTI, speedTI);
  set_motor(1, dirTD, speedTD);
  set_motor(2, dirDI, speedDI);
  set_motor(3, dirDD, speedDD);
}

void handle_obstaculos()
{
  if (digitalRead(pin_choque) == 0) {
    digitalWrite(pin_led_7colores, HIGH);
  }

  int distancia = dist_radar;

  if (distancia <= 150) {
    set_speed_rover(0);
    digitalWrite(pin_led_7colores, LOW);
  } else if (distancia <= 350) {
    set_speed_rover(1000);
  } else {
    set_speed_rover(3000);
  }
}

void task_motores(void *pvParameters)
{
  esp_task_wdt_add(NULL);

  for (int i = 0; i < 4; i++) {
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
      speed_motor.setValue(i, 1);
      digitalWriteMux(pin_dir[i][0], LOW);
      digitalWriteMux(pin_dir[i][1], LOW);
      xSemaphoreGive(i2cMutex);
    }
  }

  tipo_mov = 99;
  int last_mov = 0;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    esp_task_wdt_reset();

    if (enabled_task_motores == 1) {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
        if (tipo_mov != last_mov) {
          if ((tipo_mov == 0 || tipo_mov == 50) && control_activo != 2) {
            rover_stop();
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          }
          last_mov = tipo_mov;
        }

        if ((tipo_mov == 1) || (tipo_mov == 51)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, rover_speed, rover_speed, rover_speed, rover_speed);
        }
        else if ((tipo_mov == 2) || (tipo_mov == 52)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, rover_speed, rover_speed, rover_speed, rover_speed);
        }
        else if ((tipo_mov == 3) || (tipo_mov == 53)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, rover_speed, 0, rover_speed);
        }
        else if ((tipo_mov == 4) || (tipo_mov == 54)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, rover_speed, 0, rover_speed, 0);
        }
        else if ((tipo_mov == 5) || (tipo_mov == 55)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, 0, rover_speed, 0, rover_speed);
        }
        else if ((tipo_mov == 6) || (tipo_mov == 56)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, rover_speed, 0, rover_speed, 0);
        }
        else if ((tipo_mov == 7) || (tipo_mov == 57)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MFORWARD, MBACK, MFORWARD, rover_speed, rover_speed, rover_speed, rover_speed);
        }
        else if ((tipo_mov == 8) || (tipo_mov == 58)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MBACK, MFORWARD, MBACK, rover_speed, rover_speed, rover_speed, rover_speed);
        }
        else if ((tipo_mov == 9) || (tipo_mov == 59)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MFORWARD, MFORWARD, MBACK, rover_speed, rover_speed, rover_speed, rover_speed);
        }
        else if ((tipo_mov == 10) || (tipo_mov == 60)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MBACK, MBACK, MFORWARD, rover_speed, rover_speed, rover_speed, rover_speed);
        }
        else if (tipo_mov == 41) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, rover_speed, 0, 0, 0);
        }
        else if (tipo_mov == 42) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, rover_speed, 0, 0);
        }
        else if (tipo_mov == 43) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, 0, rover_speed, 0);
        }
        else if (tipo_mov == 44) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, 0, 0, rover_speed);
        }
        else if (tipo_mov == 45) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, rover_speed, 0, 0, 0);
        }
        else if (tipo_mov == 46) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, 0, rover_speed, 0, 0);
        }
        else if (tipo_mov == 47) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, 0, 0, rover_speed, 0);
        }
        else if (tipo_mov == 48) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, 0, 0, 0, rover_speed);
        }

        xSemaphoreGive(i2cMutex);
      }
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
  }
}