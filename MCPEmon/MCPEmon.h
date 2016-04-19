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
#ifndef MCPEmon_h
#define MCPEmon_h

#define ADC_BITS    12
#define ADC_COUNTS  (1<<ADC_BITS)

class MCPEmon
{
  public:

    MCPEmon(uint8_t clockpin, uint8_t mosipin, uint8_t misopin, uint8_t cspin);

    void voltage(unsigned int _inPinV, double _VCAL, double _PHASECAL);
    void current(unsigned int _inPinI, double _ICAL);

    void voltageTX(double _VCAL, double _PHASECAL);
    void currentTX(unsigned int _channel, double _ICAL);

    void calcVI(unsigned int crossings, unsigned int timeout);
    double calcIrms(uint8_t NUMBER_OF_SAMPLES,uint8_t channel);
    void serialprint();

    //Useful value variables
    double realPower,
           apparentPower,
           powerFactor,
           Vrms,
           Irms;


  private:
    // ADC SPI pins
    uint8_t _clockpin, _mosipin, _misopin, _cspin;
    
    // ADC Channel pins
    uint8_t _ch0,_ch1,_ch2,_ch3,_ch4,_ch5,_ch6,_ch7 = -1; 
    
    // get value from ADC channel
    uint16_t readADC(uint8_t adc_channel);

    // Supply Voltage is always 3.3v
    int SupplyVoltage = 3300;

    //Set Voltage and current input pins
    unsigned int inPinV;
    unsigned int inPinI;
    
    //Calibration coefficients
    //These need to be set in order to obtain accurate results
    double VCAL;
    double ICAL;
    double PHASECAL;

    //--------------------------------------------------------------------------------------
    // Variable declaration for emon_calc procedure
    //--------------------------------------------------------------------------------------
    int sampleV;  							 //sample_ holds the raw analog read value
    int sampleI;

    double lastFilteredV, filteredV;         //Filtered_ is the raw analog value minus the DC offset
    double filteredI;
    double offsetV;                          //Low-pass filter output
    double offsetI;                          //Low-pass filter output
    double phaseShiftedV;                             //Holds the calibrated phase shifted voltage.
    double sqV, sumV, sqI, sumI, instP, sumP;         //sq = squared, sum = Sum, inst = instantaneous
    int startV;                                       //Instantaneous voltage at start of sample window.
    boolean lastVCross, checkVCross;                  //Used to measure number of times threshold is crossed.


};

#endif


