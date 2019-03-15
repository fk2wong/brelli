// WindSense.h

#pragma once

#include <Arduino.h>
#include "Filter.h"

class WindSense
{
private:
  Filter _filter;
  int    _adcPin;

public:
  void init(int adcPin, double estDtSec);
  double getWindSpeed();
};
