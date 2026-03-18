
#include "defines.h"
#include "radio_control.h"
#include "4motores.h"

RCSwitch mySwitch = RCSwitch();
unsigned long ultima_senal_RC = 0;


// Códigos mando blanco RF433
static const int num_buttonsRC = 16;
static const unsigned long codesRC[num_buttonsRC] = {
  6047491, 6047500, 6047536, 6047692,
  6047695, 6047548, 6047551, 6047728,
  6047683, 6047503, 6047740, 6047731,
  6047743, 6047680, 6047539, 6047488
};

int decode_RC(unsigned long codeRC)
{
  for (int i = 0; i < num_buttonsRC; i++) {
    if (codesRC[i] == codeRC) {
      return i + 1;
    }
  }
  return 0;
}

int lee_RC()
{
  if (mySwitch.available()) {
    unsigned long code = mySwitch.getReceivedValue();
    mySwitch.resetAvailable();
    if (code == 0) return 0;
    return decode_RC(code);
  }
  return 0;
}

void task_radiocontrol(void *pvParameters)
{
  (void)pvParameters;

  mySwitch.enableReceive(pin_radio_control);
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    if (enabled_task_radiocontrol == 1) {
      int boton_RC = lee_RC();

      if (boton_RC >= 1 && boton_RC <= 14) {
        tipo_mov = 50 + boton_RC;
        ultima_senal_RC = millis();
        control_activo = 2;
      }

      if (boton_RC >= 11 && boton_RC <= 14) {
        switch (boton_RC) {
          case 11:
            set_speed_rover(rover_speed - 500);
            break;
          case 12:
            set_speed_rover(rover_speed + 500);
            break;
          case 13:
            digitalWrite(pin_led_7colores, HIGH);
            break;
          case 14:
            digitalWrite(pin_led_7colores, LOW);
            break;
        }
        ultima_senal_RC = millis();
        control_activo = 2;
      }

      if (millis() - ultima_senal_RC > 200 && control_activo == 2) {
        tipo_mov = 50;
        control_activo = 0;
      }
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_radiocontrol));
  }
}