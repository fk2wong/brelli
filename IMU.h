#include <stdlib.h>
#include <stdint.h>

#pragma once

typedef struct {
  double pitch;
  double roll;
} Orientation;

class IMU {
private:
  float       _alpha;
  uint8_t     _addr0bit;
  bool        _restrictPitch;
  bool        _isFirstRead;
  Orientation _lastOrient;

  void updateInternal(Orientation &orient, Orientation &raw);
  
public:
  IMU();
  void init(uint8_t addr0bit, bool restrictPitch, float filterAlpha);
  void update();
  void update(Orientation &orient);
  void update(Orientation &orient, Orientation &raw);
};
