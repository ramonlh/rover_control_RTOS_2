
#include "defines.h"
#include "mux_servos_pca9685.h"

Bonezegei_PCA9685 speed_motor(i2caddress_PCA9685);

int init_PCA9685(int freq)
{
  int auxint = speed_motor.begin();
  if (auxint == 0) {
    speed_motor.setFrequency(freq);
  }
  return auxint;
}