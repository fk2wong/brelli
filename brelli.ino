#include <Arduino.h>
#include "OpMan.h"
#include "LimitSwitch.h"
#include "PhotodiodeArray.h"
#include "Motor.h"
#include <I2C.h>

#define LOOP_MS (5)
#define PRINT_VALUES
#define DEBUG

const int frameReadyPin = A3;
const int chipSelectPin = 10;

PhotodiodeArray array1;
unsigned int integrationTime = 0x4E24;

BTDevice btDevice;
OpMan mOpMan;

void setup(){
  SPI.begin();
  I2c.begin();
  I2c.timeOut(10);
  Serial.begin(9600);

  mOpMan.init();

#ifdef DEBUG
  mOpMan.enableLimitSwitches();
#endif

  array1.init(chipSelectPin, frameReadyPin);
}

void loop(){
  BTCommand command;
  LimitSwitchState openState, tiltState;

#ifdef DEBUG
  mOpMan.pollSwitchStates(openState, tiltState);
#else

  // Get latest bluetooth command
  command = btDevice.getCommand();

  // TODO: If the wind exceeds the safe threshold, close
  
  // Close immediately if:
  // 1. The bluetooth command is BT_COMMAND_CLOSE
  // 2. Wind sensor exceeds safe threshold (TODO)
  if (command == BT_COMMAND_CLOSE)
  {
    mOpMan.emergencyClose();
  }
  else
  {
    mOpMan.processState(command);
  }
#endif

#ifdef PRINT_VALUES
  static int readCount = 0;
  if ((readCount % 25) == 0)
  {
    #ifdef DEBUG
      Serial.print("Open: "); Serial.print(openState);
      Serial.print(" Tilt: "); Serial.print(tiltState);
      Serial.println();
    #endif
  }
  readCount++;
#endif

  delay(LOOP_MS);
}
