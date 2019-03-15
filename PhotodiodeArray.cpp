#include "PhotodiodeArray.h"

PhotodiodeArray::PhotodiodeArray() :
_settings(SPI_FREQ_DEFAULT, MSBFIRST, SPI_MODE3),
_readingStarted(false)
{}

void PhotodiodeArray::init(int16_t chipSelectPin, int16_t frameReadyPin)
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

void PhotodiodeArray::startIntegration(uint16_t integrationTime)
{
  SPI.beginTransaction(_settings);
  digitalWrite(_chipSelectPin, LOW);

  SPI.transfer(MLX75306_START_INTEGRATION);
  SPI.transfer((integrationTime >> 8) & ((1 << 8) - 1));
  SPI.transfer((integrationTime) & ((1 << 8) - 1));

  digitalWrite(_chipSelectPin, HIGH);
  SPI.endTransaction();

  _readingStarted = true;
}

// -1 = integration started
// -2 = integration done, frame is not ready
// >=0 = valid centroid value
// -3 = packet corrupted
int16_t PhotodiodeArray::getCentroidReading(uint16_t integrationTime, bool startNewFrame)
{
  if (!_readingStarted)
  {
    startIntegration(integrationTime);
    return -1;
  }

  // Check frameReadyPin - threshold of 600, take an average of 5
  uint16_t frameReadiness = 0;
  uint16_t numReadings = 5;
  for (uint16_t it = 0; it < numReadings; it++)
  {
    frameReadiness += analogRead(_frameReadyPin);
  }
  if ((frameReadiness / numReadings) < 600)
  {
    return -2;
  }

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
  _readingStarted = false;
  
  if (startNewFrame)
  {
    startIntegration(integrationTime);
  }

  // Debug linear array
  /*
  for (int i = 0; i < MLX75306_READOUT_SIZE; i++)
  {
    Serial.print( rxBuffer[i]);
    Serial.print(" ");
  }
  Serial.println();*/

  uint16_t calculatedCRC = calculateCRC(rxBuffer, (MLX75306_READOUT_SIZE - 2));
  uint16_t readCRC = (rxBuffer[MLX75306_READOUT_SIZE - 2] << 8) | (rxBuffer[MLX75306_READOUT_SIZE - 1]);

  if (calculatedCRC != readCRC) 
  {
    return -3;
  }

  return (int16_t) calculateCentroid(&rxBuffer[MLX75306_READOUT_HEADER_SIZE], (MLX75306_ARRAY_END - MLX75306_ARRAY_BEGIN + 1));
}

// Calculate centroid
uint16_t PhotodiodeArray::calculateCentroid(uint8_t rxBuffer[], uint16_t length)
{
  uint32_t moments = 0;
  uint32_t area = 0;
  for (uint16_t it = 1; it <= length; it++)
  {
    moments += ((uint16_t) rxBuffer[it - 1]) * it;
    area += rxBuffer[it - 1];
  }

  return (moments / area);
}

uint16_t PhotodiodeArray::calculateCRC(uint8_t rxBuffer[], uint16_t length)
{    
  uint8_t x;
  uint16_t crc = 0x1D0F;
  
  uint8_t* dataPtr = rxBuffer;

  while (length--){
    x = crc >> 8 ^ *dataPtr++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t) x);
  }
  
  return crc;
}

// Only call if centroid reading was received
int32_t PhotodiodeArray::getErrorSum(uint8_t rxBuffer[], uint16_t length)
{
  uint32_t sumVals = 0;
  for (uint16_t it = 1; it <= length; it++)
  {
    sumVals += rxBuffer[it - 1];
  }

  int32_t average = sumVals / length;
  int32_t errorSum = 0;
  
  for (uint16_t it = 0; it < length; it++)
  {
    errorSum += abs(average - (int32_t) rxBuffer[it]);
  }

  return errorSum;
}