#include "PhotodiodeArray.h"

PhotodiodeArray::PhotodiodeArray() :
_settings(SPI_FREQ_DEFAULT, MSBFIRST, SPI_MODE3)
{}

void PhotodiodeArray::init(int chipSelectPin, int frameReadyPin)
{
  // Configure pins
  _chipSelectPin = chipSelectPin;
  _frameReadyPin = frameReadyPin;

  pinMode(_frameReadyPin, INPUT);
  pinMode(_chipSelectPin, OUTPUT);


  // Reset
  SPI.beginTransaction(_settings);
  digitalWrite(_chipSelectPin, LOW);

  SPI.transfer(MLX75306_RESET);
  SPI.transfer(0x00);
  SPI.transfer(0x00);

  digitalWrite(_chipSelectPin, HIGH);
  SPI.endTransaction();


  // Wake up
  SPI.beginTransaction(_settings);
  digitalWrite(_chipSelectPin, LOW);

  SPI.transfer(MLX75306_WAKEUP);
  SPI.transfer(0x00);
  SPI.transfer(0x00);

  digitalWrite(_chipSelectPin, HIGH);
  SPI.endTransaction();
}

// this is a blocking call
// SI - start integration - NOTE: not start integration long
// 8 bit readout only
unsigned int PhotodiodeArray::getCentroidReading(unsigned int integrationTime)
{
  // Start integration
  SPI.beginTransaction(_settings);
  digitalWrite(_chipSelectPin, LOW);

  SPI.transfer(MLX75306_START_INTEGRATION);
  SPI.transfer((integrationTime >> 8) & ((1 << 8) - 1));
  SPI.transfer((integrationTime) & ((1 << 8) - 1));

  digitalWrite(_chipSelectPin, HIGH);
  SPI.endTransaction();


  // Wait for frame ready signal
  // Could probably do a digital pin, used analog read just for sanity
  while(analogRead(_frameReadyPin) < 600){}


  // Start sensor readout - 8 bit resolution
  uint8_t rxBuffer[MLX75306_READOUT_SIZE] = {0x00};
  SPI.beginTransaction(_settings);
  digitalWrite(_chipSelectPin, LOW);

  rxBuffer[0] = SPI.transfer(MLX75306_READOUT_8_BIT);
  rxBuffer[1] = SPI.transfer(MLX75306_ARRAY_BEGIN);
  rxBuffer[2] = SPI.transfer(MLX75306_ARRAY_END);
  for (int i = 3; i < MLX75306_READOUT_SIZE; i++)
  {
    rxBuffer[i] = SPI.transfer(0x00);
  }

  digitalWrite(_chipSelectPin, HIGH);
  SPI.endTransaction();

  // Debug linear array
  for (int i = 0; i < MLX75306_READOUT_SIZE; i++)
  {
    Serial.print( rxBuffer[i]);
    Serial.print(" ");
  }
  Serial.println();


  return calculateCentroid(&rxBuffer[MLX75306_READOUT_HEADER_SIZE], (MLX75306_ARRAY_END - MLX75306_ARRAY_BEGIN + 1));
}

// Calculate centroid
unsigned int PhotodiodeArray::calculateCentroid(uint8_t rxBuffer[], unsigned int length)
{
  unsigned long moments = 0;
  unsigned long area = 0;
  for (uint16_t it = 1; it <= length; it++)
  {
    moments += ((uint16_t) rxBuffer[it - 1]) * it;
    area += rxBuffer[it - 1];
  }

  return (moments / area);
}
