

#ifndef _4MOTORES_H_
#define _4MOTORES_H_

#include <Arduino.h>

extern int rover_speed;
extern int tipo_mov;
extern volatile int control_activo;
extern const char* text_accion[10];

// Comandos base de movimiento
enum MovimientoBase {
  MOV_STOP = 0,
  MOV_ADELANTE = 1,
  MOV_ATRAS = 2,
  MOV_ADELANTE_IZQ = 3,
  MOV_ADELANTE_DER = 4,
  MOV_ATRAS_IZQ = 5,
  MOV_ATRAS_DER = 6,
  MOV_ROT_IZQ = 7,
  MOV_ROT_DER = 8,
  MOV_LAT_1 = 9,
  MOV_LAT_2 = 10,

  MOV_DI_ADELANTE = 41,
  MOV_DD_ADELANTE = 42,
  MOV_TD_ADELANTE = 43,
  MOV_TI_ADELANTE = 44,
  MOV_DI_ATRAS = 45,
  MOV_DD_ATRAS = 46,
  MOV_TD_ATRAS = 47,
  MOV_TI_ATRAS = 48
};

static constexpr int CMD_OFFSET_RADIO = 50;

// Códigos auxiliares no-motor que también metes en tipo_mov
static constexpr int CMD_CAM_IZQ   = 31;
static constexpr int CMD_CAM_DER   = 32;
static constexpr int CMD_CAM_UP    = 33;
static constexpr int CMD_CAM_DOWN  = 34;
static constexpr int CMD_SIN_MOV   = 99;

inline int comando_web(MovimientoBase mov)
{
  return static_cast<int>(mov);
}

inline int comando_radio(MovimientoBase mov)
{
  return static_cast<int>(mov) + CMD_OFFSET_RADIO;
}

inline bool comando_es_radio(int mov)
{
  return mov >= CMD_OFFSET_RADIO;
}

inline int comando_base(int mov)
{
  return comando_es_radio(mov) ? (mov - CMD_OFFSET_RADIO) : mov;
}

void digitalWriteMux(int pin, int value);
void set_motor(int motor, int dir, int speed);
void set_speed_rover(int velocidad);
void paratodo();
void rover_stop();
void rover_move(int dirDI, int dirDD, int dirTD, int dirTI,
                int speedDI, int speedDD, int speedTD, int speedTI);
void handle_obstaculos();
void task_motores(void *pvParameters);
bool rover_stop_safe();

#endif

