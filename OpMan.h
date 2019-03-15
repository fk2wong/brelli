// OpMan.h

#pragma once

#include "BTDevice.h"
#include "LimitSwitch.h"
#include "Motor.h"

typedef enum
{
  STATE_CLOSED = 0,
  STATE_OPENING = 1,
  STATE_CLOSING = 2,
  STATE_AUTO_IDLE = 3,
  STATE_AUTO_ROTATING = 4,
  STATE_AUTO_TILTING = 5,
  STATE_AUTO_STRAIGHTENING = 6,
  STATE_MANUAL_IDLE = 7,
  STATE_MANUAL_ROTATING = 8,
  STATE_MANUAL_TILTING = 9,
  STATE_MANUAL_STRAIGHTENING = 10,
  STATE_CLOSING_FROM_TILT = 11,
} OpState;


class OpMan
{
private:
  OpState     _currentState;
  LimitSwitch _limits;

  Motor _tiltMotor;
  Motor _baseMotor;

  bool  _wasInAutoMode;

  void processManualTilting(BTCommand command, bool isStraightening);
  void processManualRotating(BTCommand command);
  void processManualIdle(BTCommand command);
  void processAutoTilting(BTCommand command, bool isStraightening);
  void processAutoRotating(BTCommand command);
  void processAutoIdle(BTCommand command);
  void processOpeningClosing(BTCommand command, bool isOpening);
  void processClosed(BTCommand command);
  void processClosingFromTilt();

public:
  void init();
  void processState(BTCommand command);
  void emergencyClose();
  OpState getCurrentState();
  void setState(OpState newState);
  // For Debugging:
  void enableLimitSwitches();
  void pollSwitchStates(LimitSwitchState &openState, LimitSwitchState &tiltState);
  void disableLimitSwitches();
};
