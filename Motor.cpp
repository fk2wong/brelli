// Motor.cpp

#include "Motor.h"
#include <Arduino.h>

void Motor::init(int pwmPin, int directionPin)
{
  _pwmPin       = pwmPin;
  _directionPin = directionPin;

  pinMode(_pwmPin, OUTPUT);
  pinMode(_directionPin, OUTPUT);
  
}

void Motor::drive(int magnitude)
{
  int mappedValue;
  bool directionValue;
  
  magnitude      = max(min(magnitude, MOTOR_MAX), MOTOR_MIN);
  mappedValue    = map(abs(magnitude), 0, 100, 0, 255);
  directionValue = (magnitude > 0) ? 1 : 0;

  Serial.print("Dir: "); Serial.print(directionValue); Serial.print(" Val: "); Serial.println(mappedValue);
  digitalWrite(_directionPin, directionValue);
  analogWrite(_pwmPin, mappedValue);
}

void Motor::brake()
{
  analogWrite(_pwmPin, 0);
}
