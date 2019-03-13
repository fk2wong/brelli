// OpMan.h

#pragma once

#include "BTDevice.h"
#include "LimitSwitch.h"
#include "Motor.h"

typedef enum
{
  STATE_CLOSED,
  STATE_OPENING,
  STATE_CLOSING,
  STATE_AUTO_IDLE,
  STATE_AUTO_ROTATING,
  STATE_AUTO_TILTING,
  STATE_AUTO_STRAIGHTENING,
  STATE_MANUAL_IDLE,
  STATE_MANUAL_ROTATING,
  STATE_MANUAL_TILTING,
  STATE_MANUAL_STRAIGHTENING
} OpState;


class OpMan
{
private:
  OpState     _currentState;
  LimitSwitch _limits;

  Motor _tiltMotor;
  Motor _baseMotor;

  void processManualTilting(BTCommand command, bool isStraightening);
  void processManualRotating(BTCommand command);
  void processManualIdle(BTCommand command);
  void processAutoTilting(BTCommand command, bool isStraightening);
  void processAutoRotating(BTCommand command);
  void processAutoIdle(BTCommand command);
  void processOpeningClosing(bool isOpening);
  void processClosed(BTCommand command);

public:
  void init();
  void processState(BTCommand command);
  void emergencyClose();
  OpState getCurrentState();

  // For Debugging:
  void enableLimitSwitches();
  void pollSwitchStates(LimitSwitchState &openState, LimitSwitchState &tiltState);
  void disableLimitSwitches();
};
