
#include <Adafruit_PWMServoDriver.h>

// Dirección del PCA9685 (ajusta si es diferente)
Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver(0x40);

// Rango de pulsos del servo (ajustar según modelo)
#define SERVO_MIN  110  // Pulso mínimo (~0°)
#define SERVO_MAX  490  // Pulso máximo (~180°)

// Canales del PCA9685 para los servos
#define SERVO1_CHANNEL 8    // I/O 8 del PCA9685
#define SERVO2_CHANNEL 9    // I/O 9 del PCA9685
#define SERVO3_CHANNEL 12   // I/O 10 del PCA9685

int radar_leido=0;

int angulo_servo_1 = 90;
int angulo_servo_2 = 90;
int angulo_servo_3 = 90;
int ang_lim[16][2] = {{10,150},{10,150},{10,150},{10,150},{10,150},{10,150},{10,150},{10,150},
                      {10,150},{10,150},{30,130},{10,150},{10,150},{10,150},{10,150},{10,150}};

void moverServo(uint8_t canal, int angulo) {
  if (canal >=0 && canal <16) {
    if (angulo < ang_lim[canal][0]) angulo = ang_lim[canal][0];   // Limitar a 10°
    if (angulo > ang_lim[canal][1]) angulo = ang_lim[canal][1]; // Limitar a 150
    }
  else {
    return;
   }

  int pulsoservo = SERVO_MIN + ((angulo * (SERVO_MAX - SERVO_MIN)) / 180);
  servos.setPWM(canal, 0, pulsoservo);
}

void task_servomotores(void *pvParameters) {

  esp_task_wdt_add(NULL);
  if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
    servos.begin();
    servos.setPWMFreq(50); // 50Hz para servos
    xSemaphoreGive(i2cMutex);
    }    
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while(1) {
    esp_task_wdt_reset();
    if (enabled_task_servomotores == 1) {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
        moverServo(SERVO1_CHANNEL, angulo_servo_1);
        esp_task_wdt_reset();
        moverServo(SERVO2_CHANNEL, angulo_servo_2);
        esp_task_wdt_reset();
        if (radar_leido==1)
          {
          radar_leido=0;
          moverServo(SERVO3_CHANNEL, angulo_servo_3);
          esp_task_wdt_reset();
          }
        xSemaphoreGive(i2cMutex);
        }
      }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_servomotores));
  }

}
