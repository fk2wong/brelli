#include "IMU.h"
#include <Wire.h>
#include <math.h>
#include <Arduino.h>


#define MPU6050_ADDR(BIT0) (0x68 | (BIT0 & 0x1))  // I2C address of the MPU-6050
#define MPU6050_PWR_MGMT_1 (0x6B)
#define MPU6050_ACCEL_XOUT_H (0x3B)

//#define RAD_TO_DEG (180.0 / M_PI)

IMU::IMU()
{
}

void IMU::init(uint8_t addr0bit, bool restrictPitch, float filterAlpha)
{
  _alpha         = filterAlpha;
  _addr0bit      = addr0bit;
  _restrictPitch = restrictPitch;
  _isFirstRead   = true;
  
  Wire.beginTransmission(MPU6050_ADDR(_addr0bit));
  Wire.write(MPU6050_PWR_MGMT_1);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void IMU::updateInternal(Orientation &orient, Orientation &raw)
{
  double accX, accY, accZ;
  double pitchAxis, rollAxis, yawAxis;
  Orientation updated;
  
  Wire.beginTransmission(MPU6050_ADDR(_addr0bit));
  Wire.write(MPU6050_ACCEL_XOUT_H);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR(_addr0bit),6,true);  // request a total of 6 registers
  
  accX = (int16_t) Wire.read()<<8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  accY = (int16_t) Wire.read()<<8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  accZ = (int16_t) Wire.read()<<8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  if (_restrictPitch)
  {
//    updated.roll  = atan2(accY, accZ) * RAD_TO_DEG;
//    updated.pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;

    updated.roll  = atan2(accX, accY) * RAD_TO_DEG;
    updated.pitch = atan(-accZ / sqrt(accY * accY + accX * accX)) * RAD_TO_DEG;
  }
  else
  {
    updated.roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
    updated.pitch = atan2(-accX, accZ) * RAD_TO_DEG;
  }

  // Output the latest filtered reading
  if (_isFirstRead)
  {
    orient = updated;
    _isFirstRead = false;
  }
  else
  {
    orient.roll  = _lastOrient.roll  * (1 - _alpha) + updated.roll  * _alpha;
    orient.pitch = _lastOrient.pitch * (1 - _alpha) + updated.pitch * _alpha;
  }


  raw = updated;

  // Save new values
  _lastOrient = orient;
}

void IMU::update()
{
  Orientation unused;

  updateInternal(unused, unused);
}

void IMU::update(Orientation &orient)
{
  Orientation unused;

  updateInternal(orient, unused);
}
  
void IMU::update(Orientation &orient, Orientation &raw)
{
  updateInternal(orient, raw);
}
