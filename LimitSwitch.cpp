// LimitSwitch.cpp

#include "LimitSwitch.h"

#include <Arduino.h>
#define PRINT_IMU_VALUES

// TODO change these values
#define OPEN_ROLL_MIN (20.0)
#define OPEN_ROLL_MAX (70.0)

#define TILT_ROLL_MIN (5)
#define TILT_ROLL_MAX (25)

typedef struct IMUConfig
{
  uint8_t addr0bit;
  bool restrictPitch;
  float filterAlpha;
} IMUConfig;

IMUConfig imuConfigs[] = {
  [LIMIT_SWITCH_OPEN] = {0, true, 0.18},
  [LIMIT_SWITCH_TILT] = {1, true, 0.18},
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
  
  if (openOrient.roll > OPEN_ROLL_MAX)
  {
    // Umbrella is fully open
    openState = LIMIT_SWITCH_MAX;

    // Check tilt state
    if (tiltOrient.roll < TILT_ROLL_MIN)
    {
      tiltState = LIMIT_SWITCH_MIN;
    }
    else if (tiltOrient.roll > TILT_ROLL_MAX)
    {
      tiltState = LIMIT_SWITCH_INTERMEDIATE;
    }
    else
    {
      tiltState = LIMIT_SWITCH_MAX;
    }
  }
  else
  {
    // Check if the umbrella is slightly open or completely closed
    openState = (openOrient.roll < OPEN_ROLL_MIN) ? LIMIT_SWITCH_MIN : LIMIT_SWITCH_INTERMEDIATE;

    // The umbrella is not fully opened, so the tilt must be at 0
    tiltState = LIMIT_SWITCH_MIN;
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
