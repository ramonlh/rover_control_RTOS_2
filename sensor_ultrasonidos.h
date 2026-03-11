

const int US1_EchoPin = pin_US1_receptor;   // 34
const int US1_TriggerPin = pin_US1_emisor;  // 33

int ping(int TriggerPin, int EchoPin) {
  long duration, distanceCm;

  digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
    
  duration = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos
  
  distanceCm = duration * 10 / 292/ 2;   //convertimos a distancia, en cm
  return distanceCm;
}

int enabled_ultrasound = 0;

unsigned int dUS1;

// Definir la función que se ejecutará en la tarea
void task_ultrasonidos(void *pvParameters) {
  pinMode(US1_TriggerPin, OUTPUT);
  pinMode(US1_EchoPin, INPUT);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    if (enabled_ultrasound > 0)
      {
      // Usar el código de ping en la tarea
      long distance = ping(US1_TriggerPin, US1_EchoPin);  // Llamada a la función ping
      if (distance > 0) {
        dUS1 = distance;  // Asignar la distancia medida a la variable global
        } 
      else {
        dUS1 = 200;  // Valor inválido si no se detecta eco
        }
      }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_ultrasonidos));
  }
}
