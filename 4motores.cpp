#include "defines.h"
#include "4motores.h"
#include "mux_mcp23017.h"
#include "mux_servos_pca9685.h"
#include "radar_vl53l0x.h"

#include <Arduino.h>
#include <esp_task_wdt.h>

#include "flash_manager.h"

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
extern float angleZ;   // yaw del JY901

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

volatile bool mantener_rumbo_habilitado = false;   // flag maestro ON/OFF

static bool control_rumbo_activo = false;
static float rumbo_objetivo = 0.0f;
static float error_rumbo_prev = 0.0f;
static uint32_t t_prev_rumbo_ms = 0;

static constexpr int   RUMBO_SPEED_MIN_CONTROL = 700;   // no corregir demasiado lento
static constexpr int   RUMBO_CORRECCION_MAX    = 700;   // límite de corrección
static constexpr float RUMBO_KP                = 18.0f; // si corrige poco subir a 22-25. Si zigzaguea bajar a 12-15
static constexpr float RUMBO_DEADBAND_GRADOS   = 1.5f;  // zona muerta
static constexpr float FACTOR_CURVA = 0.45f;   // prueba 0.60; luego 0.70 si aún gira brusco

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

static inline float curva_factor_por_velocidad(int spd)
{
  spd = movement_speed(spd);

  // A baja velocidad: curva más cerrada
  const int   V_BAJA = 700;
  const float K_BAJA = 0.40f;

  // A alta velocidad: curva más suave
  const int   V_ALTA = 2500;
  const float K_ALTA = 0.60f;

  if (spd <= V_BAJA) return K_BAJA;
  if (spd >= V_ALTA) return K_ALTA;

  const float t = (float)(spd - V_BAJA) / (float)(V_ALTA - V_BAJA);
  return K_BAJA + t * (K_ALTA - K_BAJA);
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

static inline bool movimiento_recto_controlable(int mov_base)
{
  return (mov_base == MOV_ADELANTE || mov_base == MOV_ATRAS);
}

static inline float normaliza_angulo(float a)
{
  while (a > 180.0f) a -= 360.0f;
  while (a < -180.0f) a += 360.0f;
  return a;
}

static inline float error_angular(float objetivo, float actual)
{
  return normaliza_angulo(objetivo - actual);
}

// CAMBIA solo esta función para devolver el yaw real del JY901
static inline float yaw_actual_jy901()
{
  //extern volatile float angleZ;   // <-- sustituye angleZ por tu variable real
  return angleZ;                  //     (la que contiene el yaw en grados)
}

static inline void desactivar_control_rumbo()
{
  control_rumbo_activo = false;
  error_rumbo_prev = 0.0f;
  t_prev_rumbo_ms = 0;
}

static inline void activar_control_rumbo(float yaw_actual)
{
  rumbo_objetivo = yaw_actual;
  control_rumbo_activo = true;
  error_rumbo_prev = 0.0f;
  t_prev_rumbo_ms = millis();
}

static inline int calcular_correccion_rumbo(float yaw_actual)
{
  uint32_t ahora = millis();
  float dt = (t_prev_rumbo_ms == 0) ? 0.02f : (ahora - t_prev_rumbo_ms) / 1000.0f;
  if (dt < 0.005f) dt = 0.005f;
  t_prev_rumbo_ms = ahora;

  float error = error_angular(rumbo_objetivo, yaw_actual);

  if (fabsf(error) < RUMBO_DEADBAND_GRADOS) {
    error = 0.0f;
  }

  float salida = RUMBO_KP * error;
  error_rumbo_prev = error;

  if (salida > RUMBO_CORRECCION_MAX)  salida = RUMBO_CORRECCION_MAX;
  if (salida < -RUMBO_CORRECCION_MAX) salida = -RUMBO_CORRECCION_MAX;

  return (int)salida;
}

void set_mantener_rumbo(bool on)
{
  mantener_rumbo_habilitado = on;
}

bool get_mantener_rumbo()
{
  return mantener_rumbo_habilitado;
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
  const bool choque_activo = (digitalRead(pin_choque) == 0);
  flash_set_hold(choque_activo);

  const int distancia = dist_radar;
  if (distancia > 0 && distancia <= 150) {
    if (take_i2c_mutex()) {
      rover_stop();
      give_i2c_mutex();
    }
  }
}

static void apply_movement_locked(int mov_base, int spd, float yaw_actual)
{
  spd = movement_speed(spd);

  switch (mov_base) {
    case MOV_STOP:
      desactivar_control_rumbo();
      rover_stop();
      break;

    case MOV_ADELANTE:
      if (mantener_rumbo_habilitado && control_rumbo_activo && spd >= RUMBO_SPEED_MIN_CONTROL) {
        const int corr = calcular_correccion_rumbo(yaw_actual);

        const int spd_izq = clamp_speed(spd + corr);
        const int spd_der = clamp_speed(spd - corr);

        rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD,
                   spd_izq, spd_der, spd_der, spd_izq);
      } else {
        rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, spd, spd, spd, spd);
      }
      break;

    case MOV_ATRAS:
      if (mantener_rumbo_habilitado && control_rumbo_activo && spd >= RUMBO_SPEED_MIN_CONTROL) {
        const int corr = calcular_correccion_rumbo(yaw_actual);

        const int spd_izq = clamp_speed(spd - corr);
        const int spd_der = clamp_speed(spd + corr);

        rover_move(MBACK, MBACK, MBACK, MBACK,
                   spd_izq, spd_der, spd_der, spd_izq);
      } else {
        rover_move(MBACK, MBACK, MBACK, MBACK, spd, spd, spd, spd);
      }
      break;

      case MOV_ADELANTE_IZQ:
      {
        desactivar_control_rumbo();
        const float k = curva_factor_por_velocidad(spd);
        const int spd_int = clamp_speed((int)(spd * k));
        rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD,
                  spd_int, spd, spd, spd_int);
        break;
      }

      case MOV_ADELANTE_DER:
      {
        desactivar_control_rumbo();
        const float k = curva_factor_por_velocidad(spd);
        const int spd_int = clamp_speed((int)(spd * k));
        rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD,
                  spd, spd_int, spd_int, spd);
        break;
      }

      case MOV_ATRAS_IZQ:
      {
        desactivar_control_rumbo();
        const float k = curva_factor_por_velocidad(spd);
        const int spd_int = clamp_speed((int)(spd * k));
        rover_move(MBACK, MBACK, MBACK, MBACK,
                  spd_int, spd, spd, spd_int);
        break;
      }

      case MOV_ATRAS_DER:
      {
        desactivar_control_rumbo();
        const float k = curva_factor_por_velocidad(spd);
        const int spd_int = clamp_speed((int)(spd * k));
        rover_move(MBACK, MBACK, MBACK, MBACK,
                  spd, spd_int, spd_int, spd);
        break;
      }

    case MOV_ROT_DER:
      desactivar_control_rumbo();
      rover_move(MFORWARD, MBACK, MFORWARD, MBACK, spd, spd, spd, spd);
      break;

    case MOV_LAT_1:
      desactivar_control_rumbo();
      rover_move(MBACK, MFORWARD, MFORWARD, MBACK, spd, spd, spd, spd);
      break;

    case MOV_LAT_2:
      desactivar_control_rumbo();
      rover_move(MFORWARD, MBACK, MBACK, MFORWARD, spd, spd, spd, spd);
      break;

    case MOV_DI_ADELANTE:
      desactivar_control_rumbo();
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, spd, 0, 0, 0);
      break;

    case MOV_DD_ADELANTE:
      desactivar_control_rumbo();
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, spd, 0, 0);
      break;

    case MOV_TD_ADELANTE:
      desactivar_control_rumbo();
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, 0, spd, 0);
      break;

    case MOV_TI_ADELANTE:
      desactivar_control_rumbo();
      rover_move(MFORWARD, MFORWARD, MFORWARD, MFORWARD, 0, 0, 0, spd);
      break;

    case MOV_DI_ATRAS:
      desactivar_control_rumbo();
      rover_move(MBACK, MBACK, MBACK, MBACK, spd, 0, 0, 0);
      break;

    case MOV_DD_ATRAS:
      desactivar_control_rumbo();
      rover_move(MBACK, MBACK, MBACK, MBACK, 0, spd, 0, 0);
      break;

    case MOV_TD_ATRAS:
      desactivar_control_rumbo();
      rover_move(MBACK, MBACK, MBACK, MBACK, 0, 0, spd, 0);
      break;

    case MOV_TI_ATRAS:
      desactivar_control_rumbo();
      rover_move(MBACK, MBACK, MBACK, MBACK, 0, 0, 0, spd);
      break;

    default:
      desactivar_control_rumbo();
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
      const float yaw_actual = yaw_actual_jy901();
      const bool cambio_mov = (mov != last_mov);

      bool hacer_stop = false;

      if (cambio_mov) {
        if (movimiento_stop(mov) && control_activo != 2) {
          hacer_stop = true;
        }
        last_mov = mov;
      }

      if (!mantener_rumbo_habilitado || !movimiento_recto_controlable(mov_base) || spd < RUMBO_SPEED_MIN_CONTROL) {
        desactivar_control_rumbo();
      }
      else if (cambio_mov || !control_rumbo_activo) {
        activar_control_rumbo(yaw_actual);
      }

      if (take_i2c_mutex()) {
        if (hacer_stop) {
          rover_stop();
        } else {
          apply_movement_locked(mov_base, spd, yaw_actual);
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
