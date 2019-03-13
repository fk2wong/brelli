// BTDevice.cpp

#include "BTDevice.h"



char commandValues[BT_NUM_COMMANDS] = {
  [BT_COMMAND_STOP]       = '0',
  [BT_COMMAND_OPEN]       = '1',
  [BT_COMMAND_CLOSE]      = '2',
  [BT_COMMAND_TILT_UP]    = '3',
  [BT_COMMAND_TILT_DOWN]  = '4',
  [BT_COMMAND_ROTATE_CCW] = '5',
  [BT_COMMAND_ROTATE_CW]  = '6',
  [BT_COMMAND_AUTO]       = '7',
  [BT_COMMAND_MANUAL]     = '8',
};

BTCommand BTDevice::getCommand()
{
  char receivedCommandValue;

  // If a bluetooth command was received
  if (Serial.available())
  {
    receivedCommandValue = Serial.read();

    // Map the recevied raw character to a command
    for (int i = 0; i < BT_NUM_COMMANDS; i++)
    {
      if (commandValues[i] == receivedCommandValue)
      {
        // Return the command
        return i;
      }
    }
  }

  // Nothing was received, or the command was not recognized
  return BT_COMMAND_INVALID;
}

void BTDevice::flushCommands()
{
  // Read until the input buffer is empty
  while (Serial.available())
  {
    Serial.read();
  }
}
