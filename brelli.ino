#include <Arduino.h>
#include <Wire.h>
#include "LimitSwitch.h"
#include "PhotodiodeArray.h"

#define LOOP_MS (5)
#define PRINT_VALUES

const int frameReadyPin = A3;
const int chipSelectPin = 10;

PhotodiodeArray array1;
unsigned int integrationTime = 0x4E24;

LimitSwitch limits;

void setup(){
  SPI.begin();
  Wire.begin();
  Serial.begin(9600);
  
  limits.init();
  limits.enableSwitches();
  
  array1.init(chipSelectPin, frameReadyPin);
}

void loop(){
  LimitSwitchState openState, tiltState;
  
  limits.pollSwitchStates(openState, tiltState);

#ifdef PRINT_VALUES
  static int readCount = 0;
  if ((readCount % 25) == 0)
  {
    Serial.print("Open: "); Serial.print(openState);
    Serial.print(" Tilt: "); Serial.print(tiltState);
    Serial.println();
    Serial.print("Photodiode array centroid: "); Serial.println(array1.getCentroidReading(integrationTime));
  }
  readCount++;
#endif

  delay(LOOP_MS);
}
