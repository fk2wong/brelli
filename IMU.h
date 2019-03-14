#include <stdlib.h>
#include <stdint.h>
#include "Filter.h"

#pragma once

typedef struct {
  double pitch;
  double roll;
} Orientation;

class IMU {
private:
  uint8_t     _addr0bit;
  bool        _restrictPitch;

  Filter _pitchFilter;
  Filter _rollFilter;

  void updateInternal(Orientation &orient, Orientation &raw);
  
public:
  IMU();
  void init(uint8_t addr0bit, bool restrictPitch, float filterAlpha, float filterBeta, float estPollTimeSec);
  void update();
  void update(Orientation &orient);
  void update(Orientation &orient, Orientation &raw);
  void flush();
};
