#include <Arduino.h>
#include <Wire.h>
#include "IMU.h"

#define LOOP_MS (5)
#define PRINT_IMU

IMU imu;

void setup(){
  Wire.begin();
  Serial.begin(9600);
  
  imu.init(0, true, 0.18);
}

void loop(){
  Orientation current, raw;
  
  imu.update(current, raw);

#ifdef PRINT_IMU
  static int readCount = 0;
  if ((readCount % 25) == 0)
  {
    Serial.print("Roll:\t"); Serial.print(current.roll); Serial.print("\t"); Serial.print(raw.roll);
    Serial.print("\t\tPitch:\t"); Serial.print(current.pitch); Serial.print("\t"); Serial.print(raw.pitch);
    Serial.println();
  }
  readCount++;
#endif

  delay(LOOP_MS);
}
