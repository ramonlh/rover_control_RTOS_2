

#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H

#include <Arduino.h>

void init_flash_manager();
void flash_service();

void flash_pulse(uint32_t ms);     // enciende durante ms
void flash_set_hold(bool active);  // mantiene encendido mientras active=true
void flash_off_all();              // apaga todo

#endif