// Filter.cpp
#include "Filter.h"

void Filter::init(double alpha, double beta, double dt_sec)
{
  _alpha = alpha;
  _beta = beta;
  _dt = dt_sec;
  _isFirstValue = true;
}

double Filter::processValue(double value)
{
  double xk, vk, rk;
  
  if (_isFirstValue)
  {
     xk = value;
     vk = 0;
     _isFirstValue = false;
  }
  else
  {
    // State estimation
    xk = _xk_1 + _vk_1*_dt;
    vk = _vk_1;
  }

  // Error estimation
  rk = value - xk;
  
  xk += _alpha * rk;
  vk += (_beta * rk) / _dt;

  _xk_1 = xk;
  _vk_1 = vk;

  return xk;
}

void Filter::flush()
{
  _isFirstValue = true;
}
