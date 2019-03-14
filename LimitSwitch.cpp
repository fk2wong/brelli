// LimitSwitch.cpp

#include "LimitSwitch.h"

#include <Arduino.h>
#define PRINT_IMU_VALUES

// TODO change these values
#define OPEN_ROLL_OPEN_THRESHOLD   (108.3) // 108.3
#define OPEN_ROLL_CLOSED_THRESHOLD (164.0) // 169.3
#define OPEN_ROLL_HYSTERESIS       (5)

#define TILT_ROLL_MAX_TILT           (-27.0)  // -34.4
#define TILT_ROLL_STRAIGHT_THRESHOLD (1) // 1.9
#define TILT_ROLL_HYSTERESIS         (5)

#define FILTER_ALPHA (0.01) //0.01
#define FILTER_BETA  (0.00005) //0.00005
#define FILTER_EST_DT_SEC (0.005)

typedef struct IMUConfig
{
  uint8_t addr0bit;
  bool restrictPitch;
  float filterAlpha;
  float filterBeta;
  float estPollTimeSec;
} IMUConfig;

IMUConfig imuConfigs[] = {
  [LIMIT_SWITCH_OPEN] = {1, true, FILTER_ALPHA, FILTER_BETA, FILTER_EST_DT_SEC},
  [LIMIT_SWITCH_TILT] = {0, true, FILTER_ALPHA, FILTER_BETA, FILTER_EST_DT_SEC},
};

///////TEMP
static double lastValue;
//////

void LimitSwitch::init()
{
  IMUConfig *imuConfig;

  // Create IMU instances
  _openIMU = IMU();
  _tiltIMU = IMU();

  // Initialize IMUs
  imuConfig = &imuConfigs[LIMIT_SWITCH_OPEN];
  _openIMU.init(imuConfig->addr0bit, imuConfig->restrictPitch, imuConfig->filterAlpha, imuConfig->filterBeta, imuConfig->estPollTimeSec);  

  imuConfig = &imuConfigs[LIMIT_SWITCH_TILT];
  _tiltIMU.init(imuConfig->addr0bit, imuConfig->restrictPitch, imuConfig->filterAlpha, imuConfig->filterBeta, imuConfig->estPollTimeSec);  

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
  this->pollSwitchStates(openState, tiltState, 0);
}

void LimitSwitch::pollSwitchStates(LimitSwitchState &openState, LimitSwitchState &tiltState, bool useHysteresis)
{
  Orientation openOrient, tiltOrient, openRaw, tiltRaw;

  float open_closeThreshold;
  float open_openThreshold;
  float tilt_straightThreshold;
  float tilt_maxTiltThreshold;

  open_closeThreshold    = OPEN_ROLL_CLOSED_THRESHOLD;
  open_openThreshold     = OPEN_ROLL_OPEN_THRESHOLD;
  tilt_straightThreshold = TILT_ROLL_STRAIGHT_THRESHOLD;
  tilt_maxTiltThreshold  = TILT_ROLL_MAX_TILT;

  if (useHysteresis)
  {
    open_closeThreshold -= OPEN_ROLL_HYSTERESIS;
    open_openThreshold += OPEN_ROLL_HYSTERESIS;
    tilt_straightThreshold -= TILT_ROLL_HYSTERESIS;
    tilt_maxTiltThreshold += TILT_ROLL_HYSTERESIS;
  }
  
  if (!_enabled)
  {
    return;
  }

  // Update IMUs
  _openIMU.update(openOrient, openRaw);
  _tiltIMU.update(tiltOrient, tiltRaw);

  // Check open state

  if (openOrient.roll > open_closeThreshold)
  {
    openState = LIMIT_SWITCH_MIN;
  }
  else if (openOrient.roll < open_openThreshold)
  {
    openState = LIMIT_SWITCH_MAX;
  }
  else
  {
    openState = LIMIT_SWITCH_INTERMEDIATE;
  }

  if (tiltOrient.roll > tilt_straightThreshold)
  {
    tiltState = LIMIT_SWITCH_MIN;
  }
  else if (tiltOrient.roll < tilt_maxTiltThreshold)
  {
    tiltState = LIMIT_SWITCH_MIN;
  }
  else
  {
    tiltState = LIMIT_SWITCH_INTERMEDIATE;
  }

#ifdef PRINT_IMU_VALUES
lastValue = openOrient.roll;
  static int readCount = 0;
  if ((readCount % 25) == 0)
  {
    Serial.print("Open roll:\t"); Serial.print(openOrient.roll); Serial.print("\t"); Serial.print(openRaw.roll);
    Serial.print("\t\t Tilt roll:\t"); Serial.print(tiltOrient.roll); Serial.print("\t"); Serial.print(tiltRaw.pitch);
    Serial.println();
  }
  readCount++;

#endif
}

double LimitSwitch::getLastValue()
{
  return lastValue;
}
