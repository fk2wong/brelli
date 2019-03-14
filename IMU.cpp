#include "IMU.h"
#include <I2C.h>
#include <math.h>
#include <Arduino.h>


#define MPU6050_ADDR(BIT0) (0x68 | (BIT0 & 0x1))  // I2C address of the MPU-6050
#define MPU6050_PWR_MGMT_1 (0x6B)
#define MPU6050_ACCEL_XOUT_H (0x3B)

//#define RAD_TO_DEG (180.0 / M_PI)

IMU::IMU()
{
}

void IMU::init(uint8_t addr0bit, bool restrictPitch, float filterAlpha, float filterBeta, float estPollTimeSec)
{ 
  _addr0bit      = addr0bit;
  _restrictPitch = restrictPitch;

  _pitchFilter.init(filterAlpha, filterBeta, estPollTimeSec);
  _rollFilter.init(filterAlpha, filterBeta, estPollTimeSec);
  
  while (I2c.write(MPU6050_ADDR(_addr0bit), MPU6050_PWR_MGMT_1, 0) != 0)
  {
    Serial.println("I2C WRITE FAIL!");
  }
  //Wire.endTransmission(true);
}

void IMU::updateInternal(Orientation &orient, Orientation &raw)
{
  double accX, accY, accZ;
  double pitchAxis, rollAxis, yawAxis;
  Orientation newRaw;
  
  while(I2c.read(MPU6050_ADDR(_addr0bit), MPU6050_ACCEL_XOUT_H, 6) != 0 )
  {
    Serial.println("I2C READ FAIL!");
  }

  accX = (int16_t) I2c.receive()<<8 | I2c.receive();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  accY = (int16_t) I2c.receive()<<8 | I2c.receive();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  accZ = (int16_t) I2c.receive()<<8 | I2c.receive();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  if (_restrictPitch)
  {
    newRaw.roll  = atan2(accX, accY) * RAD_TO_DEG;
    newRaw.pitch = atan(-accZ / sqrt(accY * accY + accX * accX)) * RAD_TO_DEG;
  }
  else
  {
    newRaw.roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
    newRaw.pitch = atan2(-accX, accZ) * RAD_TO_DEG;
  }

  // Process filters, output
  orient.pitch  = _pitchFilter.processValue(newRaw.pitch);
  orient.roll = _rollFilter.processValue(newRaw.roll);

  raw = newRaw;
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

void IMU::flush()
{
  _rollFilter.flush();
  _pitchFilter.flush();
}
