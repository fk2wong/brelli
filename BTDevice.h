// BTDevice.h

#pragma once

#include <Arduino.h>

typedef enum BTCommand
{
  BT_COMMAND_STOP       = 0,
  BT_COMMAND_OPEN       = 1,
  BT_COMMAND_CLOSE      = 2,
  BT_COMMAND_TILT_UP    = 3,
  BT_COMMAND_TILT_DOWN  = 4,
  BT_COMMAND_ROTATE_CCW = 5,
  BT_COMMAND_ROTATE_CW  = 6,
  BT_COMMAND_AUTO       = 7,
  BT_COMMAND_MANUAL     = 8,
  BT_NUM_COMMANDS,
  BT_COMMAND_INVALID,
} BTCommand;

class BTDevice
{
public:
   BTCommand getCommand();
   void flushCommands();
};
