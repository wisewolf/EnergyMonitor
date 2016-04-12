#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "MCP3208.h"

#define SELPIN D8    //CS
#define DATAOUT D7   //MOSI
#define DATAIN D6    //MISO
#define SPICLOCK D5  //SCLK

MCP3208 adc(SPICLOCK,DATAOUT,DATAIN,SELPIN);

void setup(){
  Serial.begin(115200);
  Serial.println('Setup complete.');
}

void loop(){
  Serial.println(adc.readADC(0));
}
