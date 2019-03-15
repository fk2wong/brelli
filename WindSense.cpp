// WindSense.cpp

#include "WindSense.h"

#define WIND_ALPHA (0.1)
#define WIND_BETA (0)

void WindSense::init(int adcPin, double estDtSec)
{
  _adcPin = adcPin;
  pinMode(_adcPin, INPUT);
  
  _filter.init(WIND_ALPHA, WIND_BETA, estDtSec);
}

double WindSense::getWindSpeed()
{
  int windADunits;
  float rawWindMPH;
  float filteredWindMPH;
 
  windADunits = analogRead(_adcPin);
  rawWindMPH  = pow((((float)windADunits - 264.0) / 85.6814), 3.36814);   

  filteredWindMPH = _filter.processValue(rawWindMPH);
  //Serial.print("Filtered:\t");  Serial.print(filteredWindMPH); Serial.print(" Raw:\t"); Serial.println(rawWindMPH);
  //Serial.println(" MPH\t"); 

  return filteredWindMPH;
}
