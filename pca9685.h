
#include <Bonezegei_PCA9685.h>

// Default Address of PCA9685 = 0x40 
Bonezegei_PCA9685 speed_motor(0x40);

int init_PCA9685(int freq)
{
  int auxint = speed_motor.begin();
  if (auxint == 0) {
    // Frequency in Hertz 
    speed_motor.setFrequency(freq);
    } 
  return auxint;
}

