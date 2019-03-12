// LimitSwitch.h

#pragma once

#include "IMU.h"

typedef enum LimitSwitchType
{
  LIMIT_SWITCH_OPEN,
  LIMIT_SWITCH_TILT,
} LimitSwitchType;

typedef enum LimitSwitchState
{
  LIMIT_SWITCH_MIN,
  LIMIT_SWITCH_INTERMEDIATE,
  LIMIT_SWITCH_MAX,
} LimitSwitchState;

class LimitSwitch
{
private:
  IMU  _openIMU;
  IMU  _tiltIMU;
  bool _enabled;
  
public:
  void init();
  void enableSwitches();
  void pollSwitchStates(LimitSwitchState &openState, LimitSwitchState &tiltState);
  void disableSwitches();
};
