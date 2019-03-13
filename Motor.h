// Motor.h

#pragma once

#include <Arduino.h>

#define MOTOR_MAX 100
#define MOTOR_MIN -100

class Motor
{
private:
  int _pwmPin;
  int _directionPin;
  
public:
  void init(int pwmPin, int directionPin);

  // @param magnitude - int from -100 to 100 (MOTOR_MIN to MOTOR_MAX)
  void drive(int magnitude);
  void brake();
};
