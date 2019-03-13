// OpMan.cpp

#include "OpMan.h"

void OpMan::init()
{
  LimitSwitchState openState, tiltState;

  // Initialize limit switches
  _limits.init();

  // Enable switches, to check our current state
  _limits.enableSwitches();
  _limits.pollSwitchStates(openState, tiltState);  
  
  // Start closed
  if (openState != LIMIT_SWITCH_MIN)
  {
    // TODO start tilt motor

    _currentState = STATE_CLOSING;
  }
  else
  {
    _limits.disableSwitches();
    _currentState = STATE_CLOSED;
  }
}

OpState OpMan::getCurrentState()
{
  return _currentState;
}

void OpMan::emergencyClose()
{
  if (_currentState != STATE_CLOSED)
  {
   // TODO: Stop all motors

    // Enable limit switch sensors
    _limits.enableSwitches();
    
    // TODO: Set tilt motor
    
    _currentState = STATE_CLOSING;
  }
}

void OpMan::processState(BTCommand command)
{
  switch (_currentState)
  {
    case STATE_CLOSED:
    {
      processClosed(command);
      break;
    }
    case STATE_OPENING:
    {
      processOpeningClosing(true);
      break;
    }
    case STATE_CLOSING:
    {
      processOpeningClosing(false);
      break;
    }
    case STATE_AUTO_IDLE:
    {
      processAutoIdle(command);
      break;
    }
    case STATE_AUTO_ROTATING:
    {
      processAutoRotating(command);
      break;
    }
    case STATE_AUTO_TILTING:
    {
      processAutoTilting(command, false);
      break;
    }
    case STATE_AUTO_STRAIGHTENING:
    {
      processAutoTilting(command, true);
      break;
    }
    case STATE_MANUAL_IDLE:
    {
      processManualIdle(command);
      break;
    }
    case STATE_MANUAL_ROTATING:
    {
      processManualRotating(command);
      break;
    }
    case STATE_MANUAL_TILTING:
    {
      processManualTilting(command, false);
      break;
    }
    case STATE_MANUAL_STRAIGHTENING:
    {
      processManualTilting(command, true);
      break;
    }
    default:
    {
      Serial.println("Invalid state");
    }
  }
}

void OpMan::processClosed(BTCommand command)
{
  if (command == BT_COMMAND_OPEN)
  {
    _limits.enableSwitches();
    
    // TODO: Set tilt motor
  
    _currentState = STATE_OPENING;
  }
}

void OpMan::processOpeningClosing(bool isOpening)
{
  LimitSwitchState openState, tiltState;

  // Check if we reached the open limit
  _limits.pollSwitchStates(openState, tiltState);
  
  // If we reached a limit, change state to idle, automatic
  if (( isOpening && (openState == LIMIT_SWITCH_MAX)) ||
      (!isOpening && (openState == LIMIT_SWITCH_MIN)))
  {
    // TODO: Stop tilt motor
  
    // Turn off limit switch sensors
    _limits.disableSwitches();
  
    _currentState = STATE_AUTO_IDLE;
  }
}

void OpMan::processAutoIdle(BTCommand command)
{
  if (command == BT_COMMAND_MANUAL)
  {
    _currentState = STATE_MANUAL_IDLE;
  }
  // TODO: 
  // Else if any photodiode arrays are past threshold
  //    calculate directions to rotate/tilt
  //    start base motor
  //    set state to STATE_AUTO_ROTATING

}

void OpMan::processAutoRotating(BTCommand command)
{
  if (command == BT_COMMAND_MANUAL)
  {
    // TODO: Stop base motor
    
    _currentState = STATE_MANUAL_IDLE;
  }
  // TODO: 
  // Else if photodiode array1 is within threshold
  //    stop base motor

  _limits.enableSwitches();
  
  //    start tilt motor
  //    set state to STATE_AUTO_TILTING
  
}

void OpMan::processAutoTilting(BTCommand command, bool isStraightening)
{
  LimitSwitchState openState, tiltState;
  
  if (command == BT_COMMAND_MANUAL)
  {
    // TODO: Stop tilt motor

    _limits.disableSwitches();
    
    _currentState = STATE_MANUAL_IDLE;
  }
  else
  {
    _limits.pollSwitchStates(openState, tiltState);

    // If we reached the tilt limit, or
    // TODO: if photodiode array2 is within threshold
    if (( isStraightening && (tiltState == LIMIT_SWITCH_MIN)) ||
        (!isStraightening && (tiltState == LIMIT_SWITCH_MAX)))
    {
        // TODO: Stop tilt motor

        _limits.disableSwitches();

        _currentState = STATE_AUTO_IDLE;
    }
  }
}

void OpMan::processManualIdle(BTCommand command)
{
  if (command == BT_COMMAND_TILT_UP)
  {
    _limits.enableSwitches();
    
    // TODO: Set tilt motor

    _currentState = STATE_MANUAL_STRAIGHTENING;
  }
  if (command == BT_COMMAND_TILT_DOWN)
  {
    _limits.enableSwitches();

    // TODO: Set tilt motor

    _currentState = STATE_MANUAL_TILTING;
  }
  if (command == BT_COMMAND_ROTATE_CCW)
  {
    // TODO: Set base motor

    _currentState = STATE_MANUAL_ROTATING;
  }
  if (command == BT_COMMAND_ROTATE_CW)
  {
    // TODO: Set base motor

    _currentState = STATE_MANUAL_ROTATING;
  }
  if (command == BT_COMMAND_AUTO)
  {
    _currentState = STATE_AUTO_IDLE;
  }
}

void OpMan::processManualRotating(BTCommand command)
{
  if (command == BT_COMMAND_STOP)
  {
    // TODO: Stop base motor

    _currentState = STATE_MANUAL_IDLE;
  }
}

void OpMan::processManualTilting(BTCommand command, bool isStraightening)
{
  LimitSwitchState openState, tiltState;

  if (command == BT_COMMAND_STOP)
  {
    // TODO: Stop tilt motor

    _limits.disableSwitches();

    _currentState = STATE_MANUAL_IDLE;
  }
  else
  {
    _limits.pollSwitchStates(openState, tiltState);

    // If we reached the tilt limit, stop
    if (( isStraightening && (tiltState == LIMIT_SWITCH_MIN)) ||
        (!isStraightening && (tiltState == LIMIT_SWITCH_MAX)))
    {
        // TODO: Stop tilt motor

        _limits.disableSwitches();

        _currentState = STATE_MANUAL_IDLE;
    }
  }
}


// For Debugging:
void OpMan::enableLimitSwitches()
{
  _limits.enableSwitches();
}

void OpMan::pollSwitchStates(LimitSwitchState &openState, LimitSwitchState &tiltState)
{
  _limits.pollSwitchStates(openState, tiltState);
}

void OpMan::disableLimitSwitches()
{
  _limits.disableSwitches();
}
