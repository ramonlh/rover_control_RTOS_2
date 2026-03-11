

#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void init_vl53l0x()
{
  if (!lox.begin()) {
    Serial.println(F("Error al iniciar VL53L0X"));
    //while(1);
    }
  else {
    {
    Serial.println(F("VL53L0X OK"));
    }
  }
}

void loop_vl53l0x()
{
  VL53L0X_RangingMeasurementData_t measure;
    
  //Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance: "); Serial.print(measure.RangeMilliMeter);Serial.println(" mm");
  } else {
    Serial.println("Out of range");
  }
  delay(100);
}