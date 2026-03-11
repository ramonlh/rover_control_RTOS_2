
#include "mux_mcp23017.h"
#include <mux_servos_pca9685.h>

#define MFORWARD 1   // motor adelante
#define MBACK 0   // motor adelante

// la siguiente constante puede variar según la conexión de los motores
          // pin PWM ruedas      0=DI  2=DD   1=TI   3=TD
const uint8_t pin_pwm_motor[4] = {2,    3,     1,     0};  // salidas pwm del mux PCA9685
                            //   TD      TI      DI      DD
const uint8_t pin_dir[4][2] = {{0, 1}, {2, 3}, {4, 5}, {6, 7}}; // pines del mux mcp23017


//static Output low = Output::Low;
//static Output high = Output::High;

static int rover_speed = 1000;
//int rumbo_adelante=0;

//int speed_rover = 1000;   // para usar con la task, rover_speed se quitará
int tipo_mov = 99;     //  f, r, fl,fr,rl,rr,rotl,rotr, latl, latr 

void digitalWriteMux(int pin, int value)
{
  mcp.digitalWrite((pins)pin, value==0?LOW:HIGH);
}


void set_motor(int motor, int dir, int speed) {
  if (speed == 0) {   // stop
    digitalWriteMux(pin_dir[motor][0], HIGH);   // los dos pines a 1, inactivo
    digitalWriteMux(pin_dir[motor][1], HIGH);   // los dos pines a 1, inactivo
    speed_motor.setValue(pin_pwm_motor[motor], 0);  // velocidad a 0, parado
    } 
  else {
    digitalWriteMux(pin_dir[motor][0], (dir == 0) ? LOW : HIGH);
    digitalWriteMux(pin_dir[motor][1], (dir == 0) ? HIGH : LOW);
    speed_motor.setValue(pin_pwm_motor[motor], speed);
  }
}

void set_speed_rover(int velocidad) {
  rover_speed = constrain(velocidad, 500, 4000);
  for (int i = 0; i < 4; i++) {
    speed_motor.setValue(i, rover_speed);
  }
}
 void paratodo()
 {
  digitalWriteMux(pin_dir[0][0], HIGH);   // los dos pines a 1, inactivo
  digitalWriteMux(pin_dir[0][1], HIGH);   // los dos pines a 1, inactivo
  digitalWriteMux(pin_dir[1][0], HIGH);   // los dos pines a 1, inactivo
  digitalWriteMux(pin_dir[1][1], HIGH);   // los dos pines a 1, inactivo
  digitalWriteMux(pin_dir[2][0], HIGH);   // los dos pines a 1, inactivo
  digitalWriteMux(pin_dir[2][1], HIGH);   // los dos pines a 1, inactivo
  digitalWriteMux(pin_dir[3][0], HIGH);   // los dos pines a 1, inactivo
  digitalWriteMux(pin_dir[3][1], HIGH);   // los dos pines a 1, inactivo
  speed_motor.setValue(0, 0);
  speed_motor.setValue(1, 0);
  speed_motor.setValue(2, 0);
  speed_motor.setValue(3, 0);
 }

void rover_stop() {
  paratodo();
}

                // DI    DD     TI       TD          DI    DD     TI       TD    
void rover_move(int dirDI, int dirDD, int dirTD, int dirTI, int speedDI, int speedDD, int speedTD, int speedTI) {
  set_motor(0, dirTI, speedTI);   // Trasera derecha
  set_motor(1, dirTD, speedTD);   // Trasera izquierda
  set_motor(2, dirDI, speedDI);   // Delantera Izquierda
  set_motor(3, dirDD, speedDD);   // Delantera derecha
}

void handle_obstaculos()
  {
  // mirar la distancia del sensor de ultrasonidos
  //int distancia = dUS1;
  if (digitalRead(pin_choque)==0)
    {
    digitalWrite(pin_led_7colores, HIGH);
    }
  int distancia = dist_radar;
  //delay(5);
  //if (rumbo_adelante == 1)
    {
    if (distancia <= 150)
      {
      //rover_stop();
      //rumbo_adelante = 0;
      set_speed_rover(0);    
      digitalWrite(pin_led_7colores, LOW);
      }
    else if (distancia <= 350)
      {
      set_speed_rover(1000);    
      } 
    else if (distancia > 350)
      {
      set_speed_rover(3000); // Velocidad normal        
      }
    }
  }

const char* text_accion[10] = {"f","r","fl","fr","rl","rr","rotl","rotr","latl","latr"};
volatile int control_activo = 0; // 0: Ninguno, 1: WebSockets, 2: Radiocontrol

void task_motores(void *pvParameters) {

  esp_task_wdt_add(NULL);

  for (int i = 0; i < 4; i++) {
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
      speed_motor.setValue(i, 1);
      //mcp.digitalWrite(pin_dir[i][0], Mode::Output);
      //mcp.digitalWrite(pin_dir[i][1], Mode::Output);
      digitalWriteMux(pin_dir[i][0], LOW);
      digitalWriteMux(pin_dir[i][1], LOW);
      xSemaphoreGive(i2cMutex);
      }
    }
  //speed_rover = 1000;
  tipo_mov = 99;
  int last_mov = 0;
  int rel_giro = 4;
  
  //vTaskDelay(pdMS_TO_TICKS(1000));
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    esp_task_wdt_reset();
    if (enabled_task_motores == 1) {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) {
        if (tipo_mov != last_mov) {
          if ((tipo_mov == 0 || tipo_mov == 50) && control_activo != 2) {
            //tipo_mov=99;
            rover_stop();
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
            } 
          last_mov = tipo_mov;
          }
        // Detener el rover si el tipo_mov es 0 (WebSockets) o 50 (RF cuando no es control activo)
        //if ((tipo_mov >= 50) && ((tipo_mov <= 60))) Serial.println(text_accion[tipo_mov-50]);
        if ((tipo_mov == 1) || (tipo_mov == 51)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, rover_speed, rover_speed, rover_speed, rover_speed);  // adelante
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          } 
        else if ((tipo_mov == 2) || (tipo_mov == 52)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, rover_speed, rover_speed, rover_speed, rover_speed);  // atras
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          } 
        else if ((tipo_mov == 3) || (tipo_mov == 53)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, rover_speed, 0, rover_speed);  // adelante derecha
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          } 
        else if ((tipo_mov == 4) || (tipo_mov == 54)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, rover_speed, 0, rover_speed, 0);  // adelante izquierda
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          } 
        else if ((tipo_mov == 5) || (tipo_mov == 55)) { // atraś izquierda
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, 0, rover_speed, 0, rover_speed);  // atras izquierda
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          } 
        else if ((tipo_mov == 6) || (tipo_mov == 56)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, rover_speed, 0, rover_speed, 0);  // atras derecha
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          } 
        else if ((tipo_mov == 7) || (tipo_mov == 57)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MFORWARD, MBACK, MFORWARD,rover_speed, rover_speed, rover_speed, rover_speed);  // rot derecha
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          } 
        else if ((tipo_mov == 8) || (tipo_mov == 58)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MBACK, MFORWARD, MBACK,rover_speed, rover_speed, rover_speed, rover_speed);  // rot izquierda
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          } 
        else if ((tipo_mov == 9) || (tipo_mov == 59)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MFORWARD, MFORWARD, MBACK, rover_speed, rover_speed, rover_speed, rover_speed);  // lat izquierda
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          } 
        else if ((tipo_mov == 10) || (tipo_mov == 60)) {
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MBACK, MBACK, MFORWARD, rover_speed, rover_speed, rover_speed, rover_speed);  // lat derecha
          vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
          }

        else if (tipo_mov == 41)  { // DI
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, rover_speed, 0, 0, 0); 
          }
        else if (tipo_mov == 42)  { // DD
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, rover_speed, 0, 0); 
          }
        else if (tipo_mov == 43)  { // TI
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, 0, rover_speed, 0); 
          }
        else if (tipo_mov == 44)  { // TD
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, 0, 0, rover_speed);  
          }
        else if (tipo_mov == 45)  { // DI
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, rover_speed, 0, 0, 0);  
          }
        else if (tipo_mov == 46)  { // DD
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, 0, rover_speed, 0, 0);  // lat derecha
          }
        else if (tipo_mov == 47)  { // TI
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, 0, 0, rover_speed, 0);  // lat derecha
          }
        else if (tipo_mov == 48)  { // TD
          if (rover_speed < 500) rover_speed = 1000;
          rover_move(MBACK, MBACK, MBACK, MBACK, 0, 0, 0, rover_speed);  // lat derecha
          }
      

        xSemaphoreGive(i2cMutex); 
        }
      }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
  }

}
