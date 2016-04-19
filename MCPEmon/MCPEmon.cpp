/*

  This energy monitoring library is built upon work from these fine folks.

  --------------------------------------------------------------------------------
  Emon.cpp - Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
  GNU GPL
  modified to use up to 12 bits ADC resolution (ex. Arduino Due)
  by boredman@boredomprojects.net 26.12.2013
  Low Pass filter for offset removal replaces HP filter 1/1/2015 - RW
  --------------------------------------------------------------------------------
  MCP3208.cpp - Library for communicating with MCP3008 Analog to digital converter.
  Created by Uros Petrevski, Nodesign.net 2013
  Released into the public domain.

  ported from Python code originaly written by Adafruit learning system for rPI :
  http://learn.adafruit.com/send-raspberry-pi-data-to-cosm/python-script

  Modified to MCP3208 by JF Payeur
  --------------------------------------------------------------------------------

  Further modifications, Luís Correia and Nuno Correia
  April 2016 - for OOZLabs

*/
#include <Arduino.h>
#include "MCPEmon.h"

MCPEmon::MCPEmon(uint8_t clockpin, uint8_t mosipin, uint8_t misopin, uint8_t cspin)
{
  // define SPI outputs and inputs for bitbanging
  _cspin = cspin;
  _clockpin = clockpin;
  _mosipin = mosipin;
  _misopin = misopin;

  pinMode(_cspin, OUTPUT);
  pinMode(_clockpin, OUTPUT);
  pinMode(_mosipin, OUTPUT);
  pinMode(_misopin, INPUT);
  digitalWrite(_cspin, HIGH); //     # bring CS low
  
}

/*

//--------------------------------------------------------------------------------------
// Sets the pins to be used for voltage and current sensors
//--------------------------------------------------------------------------------------
void MCPEmon::voltage(unsigned int _inPinV, double _VCAL, double _PHASECAL)
{
  inPinV = _inPinV;
  VCAL = _VCAL;
  PHASECAL = _PHASECAL;
  offsetV = ADC_COUNTS >> 1;
}

void MCPEmon::current(unsigned int _inPinI, double _ICAL)
{
  inPinI = _inPinI;
  ICAL = _ICAL;
  offsetI = ADC_COUNTS >> 1;
}

//--------------------------------------------------------------------------------------
// Sets the pins to be used for voltage and current sensors based on emontx pin map
//--------------------------------------------------------------------------------------
void MCPEmon::voltageTX(double _VCAL, double _PHASECAL)
{
  inPinV = 2;
  VCAL = _VCAL;
  PHASECAL = _PHASECAL;
  offsetV = ADC_COUNTS >> 1;
}

void MCPEmon::currentTX(unsigned int _channel, double _ICAL)
{
  if (_channel == 1) inPinI = 3;
  if (_channel == 2) inPinI = 0;
  if (_channel == 3) inPinI = 1;
  ICAL = _ICAL;
  offsetI = ADC_COUNTS >> 1;
}

//--------------------------------------------------------------------------------------
// emon_calc procedure
// Calculates realPower,apparentPower,powerFactor,Vrms,Irms,kWh increment
// From a sample window of the mains AC voltage and current.
// The Sample window length is defined by the number of half wavelengths or crossings we choose to measure.
//--------------------------------------------------------------------------------------
void MCPEmon::calcVI(unsigned int crossings, unsigned int timeout)
{

  unsigned int crossCount = 0;                             //Used to measure number of times threshold is crossed.
  unsigned int numberOfSamples = 0;                        //This is now incremented

  //-------------------------------------------------------------------------------------------------------------------------
  // 1) Waits for the waveform to be close to 'zero' (mid-scale adc) part in sin curve.
  //-------------------------------------------------------------------------------------------------------------------------
  boolean st = false;                                //an indicator to exit the while loop

  unsigned long start = millis();    //millis()-start makes sure it doesnt get stuck in the loop if there is an error.

  while (st == false)                                //the while loop...
  {
    startV = adc.readADC(inPinV);                    //using the voltage waveform
    if ((startV < (ADC_COUNTS * 0.55)) && (startV > (ADC_COUNTS * 0.45))) st = true; //check its within range
    if ((millis() - start) > timeout) st = true;
  }

  //-------------------------------------------------------------------------------------------------------------------------
  // 2) Main measurement loop
  //-------------------------------------------------------------------------------------------------------------------------
  start = millis();

  while ((crossCount < crossings) && ((millis() - start) < timeout))
  {
    numberOfSamples++;                       //Count number of times looped.
    lastFilteredV = filteredV;               //Used for delay/phase compensation

    //-----------------------------------------------------------------------------
    // A) Read in raw voltage and current samples
    //-----------------------------------------------------------------------------
    sampleV = adc.readADC(inPinV);                 //Read in raw voltage signal
    sampleI = adc.readADC(inPinI);                 //Read in raw current signal

    //-----------------------------------------------------------------------------
    // B) Apply digital low pass filters to extract the 2.5 V or 1.65 V dc offset,
    //     then subtract this - signal is now centred on 0 counts.
    //-----------------------------------------------------------------------------
    offsetV = offsetV + ((sampleV - offsetV) / 4096);
    filteredV = sampleV - offsetV;
    offsetI = offsetI + ((sampleI - offsetI) / 4096);
    filteredI = sampleI - offsetI;

    //-----------------------------------------------------------------------------
    // C) Root-mean-square method voltage
    //-----------------------------------------------------------------------------
    sqV = filteredV * filteredV;                //1) square voltage values
    sumV += sqV;                                //2) sum

    //-----------------------------------------------------------------------------
    // D) Root-mean-square method current
    //-----------------------------------------------------------------------------
    sqI = filteredI * filteredI;                //1) square current values
    sumI += sqI;                                //2) sum

    //-----------------------------------------------------------------------------
    // E) Phase calibration
    //-----------------------------------------------------------------------------
    phaseShiftedV = lastFilteredV + PHASECAL * (filteredV - lastFilteredV);

    //-----------------------------------------------------------------------------
    // F) Instantaneous power calc
    //-----------------------------------------------------------------------------
    instP = phaseShiftedV * filteredI;          //Instantaneous Power
    sumP += instP;                              //Sum

    //-----------------------------------------------------------------------------
    // G) Find the number of times the voltage has crossed the initial voltage
    //    - every 2 crosses we will have sampled 1 wavelength
    //    - so this method allows us to sample an integer number of half wavelengths which increases accuracy
    //-----------------------------------------------------------------------------
    lastVCross = checkVCross;
    if (sampleV > startV) checkVCross = true;
    else checkVCross = false;
    if (numberOfSamples == 1) lastVCross = checkVCross;

    if (lastVCross != checkVCross) crossCount++;
  }

  //-------------------------------------------------------------------------------------------------------------------------
  // 3) Post loop calculations
  //-------------------------------------------------------------------------------------------------------------------------
  //Calculation of the root of the mean of the voltage and current squared (rms)
  //Calibration coefficients applied.

  double V_RATIO = VCAL * ((SupplyVoltage / 1000.0) / (ADC_COUNTS));
  Vrms = V_RATIO * sqrt(sumV / numberOfSamples);

  double I_RATIO = ICAL * ((SupplyVoltage / 1000.0) / (ADC_COUNTS));
  Irms = I_RATIO * sqrt(sumI / numberOfSamples);

  //Calculation power values
  realPower = V_RATIO * I_RATIO * sumP / numberOfSamples;
  apparentPower = Vrms * Irms;
  powerFactor = realPower / apparentPower;

  //Reset accumulators
  sumV = 0;
  sumI = 0;
  sumP = 0;
  //--------------------------------------------------------------------------------------
}
*/

//--------------------------------------------------------------------------------------
double MCPEmon::calcIrms(uint8_t Number_of_Samples,uint8_t channel)
{

  for (uint8_t n = 0; n < Number_of_Samples; n++)
  {
    sampleI = readADC(channel);

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset,
    //  then subtract this - signal is now centered on 0 counts.
    offsetI = (offsetI + (sampleI - offsetI) / 4096);
    filteredI = sampleI - offsetI;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum
    sumI += sqI;
  }

  double I_RATIO = ICAL * ((SupplyVoltage / 1000.0) / (ADC_COUNTS));
  Irms = I_RATIO * sqrt(sumI / Number_of_Samples);

  //Reset accumulators
  sumI = 0;
  //--------------------------------------------------------------------------------------

  return Irms;
}

// read SPI data from MCP3208 chip, 8 possible channels (0 thru 7)
uint16_t MCPEmon::readADC(uint8_t adc_channel) {

  if ((adc_channel > 7) || (adc_channel < 0)) return -1; // Wrong adc address return -1

  // algo
  digitalWrite(_cspin, HIGH);
  digitalWrite(_clockpin, LOW); //  # start clock low
  digitalWrite(_cspin, LOW); //     # bring CS low

  uint8_t commandout = adc_channel;
  commandout |= 0x18; //  # start bit + single-ended bit
  commandout <<= 3; //    # we only need to send 5 bits here

  for (uint8_t i = 0; i < 5; i++) {
    if (commandout & 0x80) {
      digitalWrite(_mosipin, HIGH);
    }
    else {
      digitalWrite(_mosipin, LOW);
    }
    commandout <<= 1;
    digitalWrite(_clockpin, HIGH);
    digitalWrite(_clockpin, LOW);

  }

  uint16_t adcout = 0;
  // read in one empty bit, one null bit and 12 ADC bits
  for (uint8_t i = 0; i < 14; i++) {
    digitalWrite(_clockpin, HIGH);
    digitalWrite(_clockpin, LOW);
    adcout <<= 1;
    if (digitalRead(_misopin))
      adcout |= 0x1;
  }
  digitalWrite(_cspin, HIGH);

  adcout >>= 1; //      # first bit is 'null' so drop it
  return adcout;
}


