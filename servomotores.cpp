
#include "defines.h"
#include "servomotores.h"

#include <Arduino.h>
#include <esp_task_wdt.h>

extern SemaphoreHandle_t i2cMutex;

Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver(0x40);

int radar_leido = 0;

int angulo_servo_1 = 90;
int angulo_servo_2 = 60;
int angulo_servo_3 = 90;

int ang_lim[16][2] = {
  {10,150},{10,150},{10,150},{10,150},
  {10,150},{10,150},{10,150},{10,150},
  {10,150},{10,150},{30,130},{10,150},
  {10,150},{10,150},{10,150},{10,150}
};

void moverServo(uint8_t canal, int angulo)
{
  if (canal < 16) {
    if (angulo < ang_lim[canal][0]) angulo = ang_lim[canal][0];
    if (angulo > ang_lim[canal][1]) angulo = ang_lim[canal][1];
  } else {
    return;
  }

  int pulsoservo = SERVO_MIN + ((angulo * (SERVO_MAX - SERVO_MIN)) / 180);
  servos.setPWM(canal, 0, pulsoservo);
}

void task_servomotores(void *pvParameters)
{
  (void)pvParameters;

  esp_task_wdt_add(NULL);

  if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
    servos.begin();
    servos.setPWMFreq(50);
    xSemaphoreGive(i2cMutex);
  }

  int last_servo_1 = -999;
  int last_servo_2 = -999;
  int last_servo_3 = -999;

  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    esp_task_wdt_reset();

    if (enabled_task_servomotores == 1) {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50))) {

        if (angulo_servo_1 != last_servo_1) {
          moverServo(SERVO1_CHANNEL, angulo_servo_1);
          last_servo_1 = angulo_servo_1;
        }

        if (angulo_servo_2 != last_servo_2) {
          moverServo(SERVO2_CHANNEL, angulo_servo_2);
          last_servo_2 = angulo_servo_2;
        }

        if (radar_leido == 1 && angulo_servo_3 != last_servo_3) {
          radar_leido = 0;
          moverServo(SERVO3_CHANNEL, angulo_servo_3);
          last_servo_3 = angulo_servo_3;
        }

        xSemaphoreGive(i2cMutex);
      }
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_servomotores));
  }
}