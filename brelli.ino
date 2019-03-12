#include <Arduino.h>
#include <Wire.h>
#include "IMU.h"
#include "PhotodiodeArray.h"

#define LOOP_MS (5)
#define PRINT_VALUES

const int frameReadyPin = A3;
const int chipSelectPin = 10;

PhotodiodeArray array1;
unsigned int integrationTime = 0x4E24;

IMU imu;

void setup(){
  SPI.begin();
  Wire.begin();
  Serial.begin(9600);
  
  imu.init(0, true, 0.18);
  array1.init(chipSelectPin, frameReadyPin);
}

void loop(){
  Orientation current, raw;
  
  imu.update(current, raw);

#ifdef PRINT_VALUES
  static int readCount = 0;
  if ((readCount % 25) == 0)
  {
    Serial.print("Roll:\t"); Serial.print(current.roll); Serial.print("\t"); Serial.print(raw.roll);
    Serial.print("\t\tPitch:\t"); Serial.print(current.pitch); Serial.print("\t"); Serial.print(raw.pitch);
    Serial.println();
    Serial.print("Photodiode array centroid: "); Serial.println(array1.getCentroidReading(integrationTime));
  }
  readCount++;
#endif

  delay(LOOP_MS);
}
