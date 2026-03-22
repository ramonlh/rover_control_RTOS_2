
#include "defines.h"
#include "flash_manager.h"

namespace {
  portMUX_TYPE flashMux = portMUX_INITIALIZER_UNLOCKED;

  volatile bool hold_active = false;
  volatile uint32_t pulse_until = 0;
  bool pin_state = false;

  void apply_output_locked() {
    const uint32_t now = millis();
    const bool pulse_active = ((int32_t)(now - pulse_until) < 0);
    const bool should_on = hold_active || pulse_active;

    if (should_on != pin_state) {
      digitalWrite(pin_led_7colores, should_on ? HIGH : LOW);
      pin_state = should_on;
    }
  }
}

void init_flash_manager() {
  pinMode(pin_led_7colores, OUTPUT);
  digitalWrite(pin_led_7colores, LOW);

  portENTER_CRITICAL(&flashMux);
  hold_active = false;
  pulse_until = 0;
  pin_state = false;
  portEXIT_CRITICAL(&flashMux);
}

void flash_pulse(uint32_t ms) {
  portENTER_CRITICAL(&flashMux);
  pulse_until = millis() + ms;
  apply_output_locked();
  portEXIT_CRITICAL(&flashMux);
}

void flash_set_hold(bool active) {
  portENTER_CRITICAL(&flashMux);
  hold_active = active;
  apply_output_locked();
  portEXIT_CRITICAL(&flashMux);
}

void flash_off_all() {
  portENTER_CRITICAL(&flashMux);
  hold_active = false;
  pulse_until = 0;
  digitalWrite(pin_led_7colores, LOW);
  pin_state = false;
  portEXIT_CRITICAL(&flashMux);
}

void flash_service() {
  portENTER_CRITICAL(&flashMux);
  apply_output_locked();
  portEXIT_CRITICAL(&flashMux);
}