
#include "defines.h"
#include "sensor_ultrasonidos.h"

const int US1_EchoPin = pin_US1_receptor;
const int US1_TriggerPin = pin_US1_emisor;

int enabled_ultrasound = 0;
unsigned int dUS1 = 0;

int ping(int TriggerPin, int EchoPin)
{
  long duration, distanceCm;

  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  duration = pulseIn(EchoPin, HIGH);
  distanceCm = duration * 10 / 292 / 2;
  return distanceCm;
}

void task_ultrasonidos(void *pvParameters)
{
  (void)pvParameters;

  pinMode(US1_TriggerPin, OUTPUT);
  pinMode(US1_EchoPin, INPUT);

  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    if (enabled_ultrasound > 0) {
      long distance = ping(US1_TriggerPin, US1_EchoPin);

      if (distance > 0) {
        dUS1 = distance;
      } else {
        dUS1 = 200;
      }
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_ultrasonidos));
  }
}