// Filter.h

#pragma once

#include <Arduino.h>

class Filter
{
private:
  bool _isFirstValue;
  double _alpha;
  double _beta;
  double _dt;
  
  double _xk_1;
  double _vk_1;

public:
  void init(double alpha, double beta, double dt_sec);
  double processValue(double value);
  void flush();
};
