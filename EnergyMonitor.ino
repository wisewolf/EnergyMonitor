/*
  Example code to get a MCP3*08 running with an ESP8266
  for DiY energy monitoring solutions
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "MCP3208.h"
#include "EmonLib.h"

#define SELPIN D8    //CS
#define DATAOUT D7   //MOSI
#define DATAIN D6    //MISO
#define SPICLOCK D5  //SCLK

MCP3208 adc(SPICLOCK,DATAOUT,DATAIN,SELPIN);
EnergyMonitor emon1;

// ADC read time "delays"
unsigned long adcTime;
unsigned long adcDelay = 1000;

// stub function for resetting ADC read time delay
void setAdcDelay() {
  adcTime = millis() + adcDelay;
}

void setup(){
  Serial.begin(115200);
  emon1.current(0, 111.1);             // Current: ADC Channel, calibration.
  setAdcDelay();
}

void loop(){

  if (millis() >= adcTime) {
	  Serial.println(adc.readADC(0));
    double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    Serial.print(Irms*230.0);	       // Apparent power
    Serial.print(" ");
    Serial.println(Irms);		       // Irms
    setAdcDelay();
 }

}
