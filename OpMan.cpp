// OpMan.cpp

#include "OpMan.h"

#define BASE_MOTOR_DIRECTION_PIN (2)
#define BASE_MOTOR_PWM_PIN       (3)

#define TILT_MOTOR_DIRECTION_PIN (4)
#define TILT_MOTOR_PWM_PIN       (5)

#define TILT_MOTOR_OPEN_SPEED       ( 100)
#define TILT_MOTOR_CLOSE_SPEED      (-100)
#define TILT_MOTOR_TILT_SPEED       ( 35 )
#define TILT_MOTOR_STRAIGHTEN_SPEED (-35 )
#define BASE_MOTOR_CW_SPEED         ( 25 )
#define BASE_MOTOR_CCW_SPEED        (-25 )

void OpMan::init()
{
  LimitSwitchState openState, tiltState;

  // Initialize limit switches
  _limits.init();
  
  // Initialize motors
  _tiltMotor.init(TILT_MOTOR_PWM_PIN, TILT_MOTOR_DIRECTION_PIN);
  _baseMotor.init(BASE_MOTOR_PWM_PIN, BASE_MOTOR_DIRECTION_PIN);

  // Enable switches, to check our current state
  _limits.enableSwitches();
  _limits.pollSwitchStates(openState, tiltState, 1);  

//  //////////
//  while(1)
//  {
//      static int state = -1;
//      _limits.pollSwitchStates(openState, tiltState); 
//
//      if (tiltState != state)
//      {
//        Serial.println(tiltState);
//        Serial.print("Last value: "); Serial.println(_limits.getLastValue());
//        state = tiltState;
//      }
////      Serial.print("Open: "); Serial.print(openState);
////      Serial.print(" Tilt: "); Serial.print(tiltState);
////      Serial.println(); 
//      delay(5);
//  }
//  ///////////
  
  // Close the umbrella initially, if not already 
  
  // If tilted, then we're closing from tilt
  if (tiltState != LIMIT_SWITCH_MIN)
  {
    _tiltMotor.drive(TILT_MOTOR_STRAIGHTEN_SPEED);
    this->setState(STATE_CLOSING_FROM_TILT);
  }
  else if (openState != LIMIT_SWITCH_MIN)
  {
    _tiltMotor.drive(TILT_MOTOR_CLOSE_SPEED);
    this->setState(STATE_CLOSING);
  }
  else
  {
    _limits.disableSwitches();
    this->setState(STATE_CLOSED);
  }

  _wasInAutoMode = false;
}

OpState OpMan::getCurrentState()
{
  return _currentState;
}

void OpMan::emergencyClose()
{
  LimitSwitchState openState, tiltState;
  
  if (_currentState != STATE_CLOSED)
  {
    _tiltMotor.brake();
    _baseMotor.brake();

    // Enable limit switch sensors
    _limits.enableSwitches();

    // Read if we're tilted
    _limits.pollSwitchStates(openState, tiltState, 0);

    // If tilted, then we're closing from tilt
    if (tiltState != LIMIT_SWITCH_MIN)
    {
      _tiltMotor.drive(TILT_MOTOR_STRAIGHTEN_SPEED);
      this->setState(STATE_CLOSING_FROM_TILT);
    }
    else
    {
      // Start closing the umbrella
      _tiltMotor.drive(TILT_MOTOR_CLOSE_SPEED);
      this->setState(STATE_CLOSING);
    }
  }
}

void OpMan::processState(BTCommand command)
{
  // Operate in the current state, with optional new command input
  switch (_currentState)
  {
    case STATE_CLOSED:
    {
      processClosed(command);
      break;
    }
    case STATE_OPENING:
    {
      processOpeningClosing(command, true);
      break;
    }
    case STATE_CLOSING:
    {
      processOpeningClosing(command, false);
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
    case STATE_CLOSING_FROM_TILT:
    {
      processClosingFromTilt();
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
    // Enable limit switches
    _limits.enableSwitches();

    // Start opening umbrella
    _tiltMotor.drive(TILT_MOTOR_OPEN_SPEED);
    this->setState(STATE_OPENING);
  }
}

void OpMan::processOpeningClosing(BTCommand command, bool isOpening)
{
  LimitSwitchState openState, tiltState;

  if (isOpening && (command == BT_COMMAND_CLOSE))
  {
    _tiltMotor.drive(TILT_MOTOR_CLOSE_SPEED);
    this->setState(STATE_CLOSING);
  }
  else if (!isOpening && (command == BT_COMMAND_OPEN))
  {
    _tiltMotor.drive(TILT_MOTOR_OPEN_SPEED);
    this->setState(STATE_OPENING);
  }

  // Check if we reached the open limit
  _limits.pollSwitchStates(openState, tiltState);
  
  // If we reached a limit, change state to idle, automatic
  if (( isOpening && (openState == LIMIT_SWITCH_MAX)) ||
      (!isOpening && (openState == LIMIT_SWITCH_MIN)))
  {
    // Stop motor
    _tiltMotor.brake();
  
    // Turn off limit switch sensors
    _limits.disableSwitches();

    // After opening, default state is manual/idle
    if (isOpening)
    {
      this->setState(_wasInAutoMode ? STATE_AUTO_IDLE : STATE_MANUAL_IDLE);
    }
    else
    {
      this->setState(STATE_CLOSED);
    }
  }
}

void OpMan::processAutoIdle(BTCommand command)
{
  if (command == BT_COMMAND_MANUAL)
  {
    this->setState(STATE_MANUAL_IDLE);
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
    // Stop base motor
    _baseMotor.brake();
    this->setState(STATE_MANUAL_IDLE);
  }
  // TODO: 
  // Else if photodiode array1 is within threshold
  //    stop base motor

  _limits.enableSwitches();
  
  //    After finding the correct rotation, drive the tilt axis towards the sun
  //    TODO: calc direction based on photo arrays
  //    drive tilt motor
  //    set state to STATE_AUTO_TILTING
  
}

void OpMan::processAutoTilting(BTCommand command, bool isStraightening)
{
  LimitSwitchState openState, tiltState;
  
  if (command == BT_COMMAND_MANUAL)
  {
    // Stop base motor
    _baseMotor.brake();

    // Disable limit switches
    _limits.disableSwitches();
    this->setState(STATE_MANUAL_IDLE);
  }
  else
  {
    _limits.pollSwitchStates(openState, tiltState);

    // If we reached the tilt limit, or
    // TODO: if photodiode array2 is within threshold
    if (( isStraightening && (tiltState == LIMIT_SWITCH_MIN)) ||
        (!isStraightening && (tiltState == LIMIT_SWITCH_MAX)))
    {
        // Stop tilt motor
        _tiltMotor.brake();

        // Disable limit switches
        _limits.disableSwitches();
        this->setState(STATE_AUTO_IDLE);
    }
  }
}

void OpMan::processManualIdle(BTCommand command)
{
  if (command == BT_COMMAND_STRAIGHTEN)
  {
    _limits.enableSwitches();
    
    // Start tilt motor
    _tiltMotor.drive(TILT_MOTOR_STRAIGHTEN_SPEED);
    this->setState(STATE_MANUAL_STRAIGHTENING);
  }
  else if (command == BT_COMMAND_TILT)
  {
    _limits.enableSwitches();

    // Start tilt motor
    _tiltMotor.drive(TILT_MOTOR_TILT_SPEED);
    this->setState(STATE_MANUAL_TILTING);
  }
  else if (command == BT_COMMAND_ROTATE_CCW)
  {
    // Start base motor
    _baseMotor.drive(BASE_MOTOR_CCW_SPEED);
    this->setState(STATE_MANUAL_ROTATING);
  }
  else if (command == BT_COMMAND_ROTATE_CW)
  {
    // Start base motor
    _baseMotor.drive(BASE_MOTOR_CW_SPEED);
    this->setState(STATE_MANUAL_ROTATING);
  }
  else if (command == BT_COMMAND_AUTO)
  {
    this->setState(STATE_AUTO_IDLE);
  }
}

void OpMan::processManualRotating(BTCommand command)
{
  if (command == BT_COMMAND_STOP)
  {
    // Stop base motor
    _baseMotor.brake();
    this->setState(STATE_MANUAL_IDLE);
  }
}

void OpMan::processManualTilting(BTCommand command, bool isStraightening)
{
  LimitSwitchState openState, tiltState;

  if (command == BT_COMMAND_STOP)
  {
    // Stop tilt motor
    _tiltMotor.brake();
    _limits.disableSwitches();

    this->setState(STATE_MANUAL_IDLE);
  }
  else
  {
    _limits.pollSwitchStates(openState, tiltState);

    // If we reached the tilt limit, stop
    if (( isStraightening && (tiltState == LIMIT_SWITCH_MIN)) ||
        (!isStraightening && (tiltState == LIMIT_SWITCH_MAX)))
    {
        // Stop tilt motor
        _tiltMotor.brake();

        _limits.disableSwitches();

        this->setState(STATE_MANUAL_IDLE);
    }
  }
}

void OpMan::processClosingFromTilt()
{
  LimitSwitchState openState, tiltState;
 
  _limits.pollSwitchStates(openState, tiltState);

  // If we reached the tilt limit, stop
  if (tiltState == LIMIT_SWITCH_MIN)
  {
      // Stop tilt motor
      _tiltMotor.brake();

      // Close normally
      _tiltMotor.drive(TILT_MOTOR_CLOSE_SPEED);
      this->setState(STATE_CLOSING);
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

void OpMan::setState(OpState newState)
{
  if (_currentState != newState)
  {
    _currentState = newState;
    Serial.print("State: "); Serial.println(_currentState);

    if (_currentState == STATE_MANUAL_IDLE){
      _wasInAutoMode = false;
    }
    else if (_currentState == STATE_AUTO_IDLE)
    {
      _wasInAutoMode = true;
    }
  }
}
