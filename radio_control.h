
#ifndef RADIO_CONTROL_H
#define RADIO_CONTROL_H

#include <Arduino.h>
#include <RCSwitch.h>

extern RCSwitch mySwitch;
extern unsigned long ultima_senal_RC;

int decode_RC(unsigned long codeRC);
int lee_RC();
void task_radiocontrol(void *pvParameters);

#endif