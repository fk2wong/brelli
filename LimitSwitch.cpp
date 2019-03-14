// LimitSwitch.cpp

#include "LimitSwitch.h"

#include <Arduino.h>
#define PRINT_IMU_VALUES

// TODO change these values
#define OPEN_ROLL_OPEN_THRESHOLD (108.3) // 108.3
#define OPEN_ROLL_CLOSED_THRESHOLD (168.0) // 169.3

#define TILT_ROLL_MAX_TILT (-27.0)  // -34.4
#define TILT_ROLL_STRAIGHT_THRESHOLD (1) // 1.9

typedef struct IMUConfig
{
  uint8_t addr0bit;
  bool restrictPitch;
  float filterAlpha;
} IMUConfig;

IMUConfig imuConfigs[] = {
  [LIMIT_SWITCH_OPEN] = {1, true, 0.05},
  [LIMIT_SWITCH_TILT] = {0, true, 0.05},
};

void LimitSwitch::init()
{
  IMUConfig *imuConfig;

  // Create IMU instances
  _openIMU = IMU();
  _tiltIMU = IMU();

  // Initialize IMUs
  imuConfig = &imuConfigs[LIMIT_SWITCH_OPEN];
  _openIMU.init(imuConfig->addr0bit, imuConfig->restrictPitch, imuConfig->filterAlpha); 

  imuConfig = &imuConfigs[LIMIT_SWITCH_TILT];
  _tiltIMU.init(imuConfig->addr0bit, imuConfig->restrictPitch, imuConfig->filterAlpha);  

  _enabled = false;
}

void LimitSwitch::enableSwitches()
{
  _openIMU.flush();
  _tiltIMU.flush();

  _enabled = true;
}

void LimitSwitch::disableSwitches()
{
  _enabled = false;
}

void LimitSwitch::pollSwitchStates(LimitSwitchState &openState, LimitSwitchState &tiltState)
{
  Orientation openOrient, tiltOrient;

  if (!_enabled)
  {
    return;
  }

  // Update IMUs
  _openIMU.update(openOrient);
  _tiltIMU.update(tiltOrient);

  // Check open state

  if (tiltOrient.roll < TILT_ROLL_STRAIGHT_THRESHOLD)
  {
    // If we're tilting, then we're already open
    openState = LIMIT_SWITCH_MAX;

    tiltState = (tiltOrient.roll > TILT_ROLL_MAX_TILT) ? LIMIT_SWITCH_INTERMEDIATE : LIMIT_SWITCH_MAX;
  }
  else
  {
    tiltState = LIMIT_SWITCH_MIN;

    if (openOrient.roll > OPEN_ROLL_CLOSED_THRESHOLD)
    {
      openState = LIMIT_SWITCH_MIN;
    }
    else if (openOrient.roll < OPEN_ROLL_OPEN_THRESHOLD)
    {
      openState = LIMIT_SWITCH_MAX;
    }
    else
    {
      openState = LIMIT_SWITCH_INTERMEDIATE;
    }
  }

#ifdef PRINT_IMU_VALUES
  static int readCount = 0;
  if ((readCount % 25) == 0)
  {
    Serial.print("Open roll:\t"); Serial.print(openOrient.roll); //Serial.print("\t"); Serial.print(raw.roll);
    Serial.print("\t\t Tilt roll:\t"); Serial.print(tiltOrient.roll); //Serial.print("\t"); Serial.print(raw.pitch);
    Serial.println();
  }
  readCount++;
#endif
}
