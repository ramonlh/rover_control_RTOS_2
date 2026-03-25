
#include "defines.h"
#include "radio_control.h"
#include "4motores.h"

RCSwitch mySwitch = RCSwitch();
unsigned long ultima_senal_RC = 0;

// Códigos mando RF433 de 18 teclas
static const int num_buttonsRC = 18;
static const unsigned long codesRC[num_buttonsRC] = {
  7450371, // 1
  7450380, // 2
  7450416, // 3
  7450572, // 4
  7450575, // 5
  7450428, // 6
  7450431, // 7
  7450608, // 8
  7450563, // 9
  7450383, // 10
  7450620, // 11
  7450611, // 12
  7450623, // 13
  7450560, // 14
  7450419, // 15
  7450368, // 16
  7450432, // 17
  7450384  // 18
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
      const int boton_RC = lee_RC();

      switch (boton_RC) {
        case 1:   // + velocidad
          set_speed_rover(rover_speed + 500);
          break;

        case 2:   // - velocidad
          set_speed_rover(rover_speed - 500);
          break;

        case 3:   // luces
          digitalWrite(pin_led_izquierda, !digitalRead(pin_led_izquierda));
          digitalWrite(pin_led_derecha, !digitalRead(pin_led_derecha));
          break;

        case 4:   // flash
          digitalWrite(pin_led_7colores, !digitalRead(pin_led_7colores));
          break;

        case 5:   // activar/desactivar rumbo
          digitalWrite(pin_led_izquierda, get_mantener_rumbo());
          digitalWrite(pin_led_derecha, get_mantener_rumbo());        
          set_mantener_rumbo(!get_mantener_rumbo());
          break;

        case 6:   // STOP
          tipo_mov = comando_radio(MOV_STOP);
          ultima_senal_RC = millis();
          control_activo = 0;
          break;

        case 7:   // giro izda
          tipo_mov = comando_radio(MOV_ADELANTE_IZQ);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        case 8:   // adelante
          tipo_mov = comando_radio(MOV_ADELANTE);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        case 9:   // giro dcha
          tipo_mov = comando_radio(MOV_ADELANTE_DER);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        case 10:  // lat izda
          tipo_mov = comando_radio(MOV_LAT_1);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        case 11:  // sin uso
          break;

        case 12:  // lat dcha
          tipo_mov = comando_radio(MOV_LAT_2);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        case 13:  // rot izda
          tipo_mov = comando_radio(MOV_ROT_IZQ);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        case 14:  // sin uso
          break;

        case 15:  // rot dcha
          tipo_mov = comando_radio(MOV_ROT_DER);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        case 16:  // giro izda atrás
          tipo_mov = comando_radio(MOV_ATRAS_IZQ);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        case 17:  // atrás recto
          tipo_mov = comando_radio(MOV_ATRAS);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        case 18:  // giro dcha atrás
          tipo_mov = comando_radio(MOV_ATRAS_DER);
          ultima_senal_RC = millis();
          control_activo = 2;
          break;

        default:
          break;
      }

      if (control_activo == 2 && (millis() - ultima_senal_RC > 200)) {
        tipo_mov = comando_radio(MOV_STOP);
        control_activo = 0;
      }
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(periodo_task_radiocontrol));
  }
}
