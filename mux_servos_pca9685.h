
#ifndef MUX_SERVOS_PCA9685_H
#define MUX_SERVOS_PCA9685_H

#include <Bonezegei_PCA9685.h>

extern Bonezegei_PCA9685 speed_motor;

int init_PCA9685(int freq);

#endif