#include "defines.h"
#include "4motores.h"
#include "mux_mcp23017.h"
#include "mux_servos_pca9685.h"
#include "radar_vl53l0x.h"

#include <Arduino.h>
#include <esp_task_wdt.h>

#define MFORWARD 1
#define MBACK    0

static constexpr int NUM_MOTORES = 4;
static constexpr int I2C_MUTEX_TIMEOUT_MS = 100;

static constexpr int ROVER_SPEED_MIN = 0;
static constexpr int ROVER_SPEED_MAX = 4000;
static constexpr int ROVER_SPEED_MIN_MOV = 500;
static constexpr int ROVER_SPEED_DEFAULT_MOV = 1000;

static constexpr int MOV_INVALID = -1;

extern SemaphoreHandle_t i2cMutex;

const uint8_t pin_pwm_motor[NUM_MOTORES] = {2, 3, 1, 0};
const uint8_t pin_dir[NUM_MOTORES][2] = {
  {0, 1},
  {2, 3},
  {4, 5},
  {6, 7}
};

int rover_speed = ROVER_SPEED_DEFAULT_MOV;
int tipo_mov = CMD_SIN_MOV;
volatile int control_activo = 0;
const char* text_accion[10] = {"f","r","fl","fr","rl","rr","rotl","rotr","latl","latr"};

// Orden lógico en rover_move():
// rover_move(dirDI, dirDD, dirTD, dirTI, speedDI, speedDD, speedTD, speedTI)
//
// Mapeo físico real:
// motor 0 -> TI
// motor 1 -> TD
// motor 2 -> DI
// motor 3 -> DD

static inline int clamp_speed(int speed)
{
  return constrain(speed, ROVER_SPEED_MIN, ROVER_SPEED_MAX);
}

static inline int movement_speed(int speed)
{
  speed = clamp_speed(speed);

  if (speed == 0) {
    return 0;
  }

  if (speed < ROVER_SPEED_MIN_MOV) {
    return ROVER_SPEED_MIN_MOV;
  }

  return speed;
}

static inline bool take_i2c_mutex(TickType_t timeoutTicks = pdMS_TO_TICKS(I2C_MUTEX_TIMEOUT_MS))
{
  return xSemaphoreTake(i2cMutex, timeoutTicks) == pdTRUE;
}

static inline void give_i2c_mutex()
{
  xSemaphoreGive(i2cMutex);
}

static inline bool movimiento_desde_radio(int mov)
{
  return (mov >= CMD_OFFSET_RADIO);
}

static inline int movimiento_base(int mov)
{
  if (mov >= CMD_OFFSET_RADIO) {
    return mov - CMD_OFFSET_RADIO;
  }
  return mov;
}

static inline bool movimiento_stop(int mov)
{
  return (mov == MOV_STOP || mov == (MOV_STOP + CMD_OFFSET_RADIO));
}

void digitalWriteMux(int pin, int value)
{
  mcp.digitalWrite((pins)pin, value == 0 ? LOW : HIGH);
}

void set_motor(int motor, int dir, int speed)
{
  speed = clamp_speed(speed);

  if (speed == 0) {
    digitalWriteMux(pin_dir[motor][0], HIGH);
    digitalWriteMux(pin_dir[motor][1], HIGH);
    speed_motor.setValue(pin_pwm_motor[motor], 0);
    return;
  }

  digitalWriteMux(pin_dir[motor][0], (dir == MBACK) ? LOW : HIGH);
  digitalWriteMux(pin_dir[motor][1], (dir == MBACK) ? HIGH : LOW);
  speed_motor.setValue(pin_pwm_motor[motor], speed);
}

void set_speed_rover(int velocidad)
{
  rover_speed = clamp_speed(velocidad);

  for (int i = 0; i < NUM_MOTORES; i++) {
    speed_motor.setValue(pin_pwm_motor[i], rover_speed);
  }
}

void paratodo()
{
  for (int i = 0; i < NUM_MOTORES; i++) {
    digitalWriteMux(pin_dir[i][0], HIGH);
    digitalWriteMux(pin_dir[i][1], HIGH);
    speed_motor.setValue(pin_pwm_motor[i], 0);
  }
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

  const int distancia = dist_radar;
  if (distancia > 0 && distancia <= 150) {
    if (take_i2c_mutex()) {
      rover_stop();
      give_i2c_mutex();
    }

    digitalWrite(pin_led_7colores, LOW);
  }
}

static void apply_movement_locked(int mov_base, int spd)
{
  spd = movement_speed(spd);
  switch (mov_base) {
    case MOV_STOP:
      rover_stop();
      break;

    case MOV_ADELANTE:
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, spd, spd, spd, spd);
      break;

    case MOV_ATRAS:
      rover_move(MBACK, MBACK, MBACK, MBACK, spd, spd, spd, spd);
      break;

    case MOV_ADELANTE_IZQ:
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, spd, 0, spd);
      break;

    case MOV_ADELANTE_DER:
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, spd, 0, spd, 0);
      break;

    case MOV_ATRAS_IZQ:
      rover_move(MBACK, MBACK, MBACK, MBACK, 0, spd, 0, spd);
      break;

    case MOV_ATRAS_DER:
      rover_move(MBACK, MBACK, MBACK, MBACK, spd, 0, spd, 0);
      break;

    case MOV_ROT_IZQ:
      rover_move(MBACK, MFORWARD, MBACK, MFORWARD, spd, spd, spd, spd);
      break;

    case MOV_ROT_DER:
      rover_move(MFORWARD, MBACK, MFORWARD, MBACK, spd, spd, spd, spd);
      break;

    case MOV_LAT_1:
      rover_move(MBACK, MFORWARD, MFORWARD, MBACK, spd, spd, spd, spd);
      break;

    case MOV_LAT_2:
      rover_move(MFORWARD, MBACK, MBACK, MFORWARD, spd, spd, spd, spd);
      break;

    case MOV_DI_ADELANTE:
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, spd, 0, 0, 0);
      break;

    case MOV_DD_ADELANTE:
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, spd, 0, 0);
      break;

    case MOV_TD_ADELANTE:
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, 0, spd, 0);
      break;

    case MOV_TI_ADELANTE:
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, 0, 0, spd);
      break;

    case MOV_DI_ATRAS:
      rover_move(MBACK, MBACK, MBACK, MBACK, spd, 0, 0, 0);
      break;

    case MOV_DD_ATRAS:
      rover_move(MBACK, MBACK, MBACK, MBACK, 0, spd, 0, 0);
      break;

    case MOV_TD_ATRAS:
      rover_move(MBACK, MBACK, MBACK, MBACK, 0, 0, spd, 0);
      break;

    case MOV_TI_ATRAS:
      rover_move(MBACK, MBACK, MBACK, MBACK, 0, 0, 0, spd);
      break;

    default:
      rover_stop();
      break;
  }
}

bool rover_stop_safe()
{
  if (take_i2c_mutex()) {
    rover_stop();
    give_i2c_mutex();
    return true;
  }
  return false;
}

void task_motores(void *pvParameters)
{
  esp_task_wdt_add(NULL);

  if (take_i2c_mutex()) {
    rover_stop();
    give_i2c_mutex();
  }

  tipo_mov = CMD_SIN_MOV;
  int last_mov = MOV_INVALID;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    esp_task_wdt_reset();
    if (enabled_task_motores == 1) {
      const int mov = tipo_mov;
      const bool desde_radio = movimiento_desde_radio(mov);
      const int mov_base = movimiento_base(mov);
      const int spd = rover_speed;

      bool hacer_stop = false;

      if (mov != last_mov) {
        if (movimiento_stop(mov) && control_activo != 2) {
          hacer_stop = true;
        }
        last_mov = mov;
      }

      if (take_i2c_mutex()) {
        if (hacer_stop) {
          rover_stop();
        } else {
          apply_movement_locked(mov_base, spd);
        }
        give_i2c_mutex();
      }

      (void)desde_radio;
    }
    else {
      if (take_i2c_mutex()) {
        rover_stop();
        give_i2c_mutex();
      }
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_motores));
  }
}
