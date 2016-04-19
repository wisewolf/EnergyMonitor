/*
  Example code to get a MCP3*08 running with an ESP8266
  for DiY energy monitoring solutions
 */

#include <ESP8266WiFi.h>
#include "EmonLib.h"
#include "config.h"

const char* ssid = "...";
const char* password = "...";
//const char* host = "emoncms.org";
const int   httpPort = 80;
const char* emoncmsKey = "...";   //emoncms.org write API key
const char* node = "1";           //This should be an integer
const int   Vrms = 230;

MCP3208 adc(SPICLOCK, DATAOUT, DATAIN, SELPIN);
EnergyMonitor emon1, emon2, emon3;

// ADC read time "delays"
unsigned long adcTime;
unsigned long adcDelay = 5000;

// stub function for resetting ADC read time delay
void setAdcDelay() {
  adcTime = millis() + adcDelay;
}

void sendToEmonCMS(String nodeId, String data) {
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    return;
  }

  // We now create a URI for the request
  String url = "/input/post.json?node=";
  url += nodeId;
  url += "&apikey=";
  url += emoncmsKey;
  url += "&csv=";
  url += data;

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server
  while (client.available()) {
    String line = client.readStringUntil('\r');
  }

}

void setup(){
  emon1.current(0, 111.1,adc);             // Current: ADC Channel, calibration.
  emon2.current(1, 111.1,adc);             // Current: ADC Channel, calibration.
  emon3.current(2, 111.1,adc);             // Current: ADC Channel, calibration.
  setAdcDelay();
}

void loop(){

  if (millis() >= adcTime) {
    double Irms0 = emon1.calcIrms(1480);  // Calculate Irms only
    double Irms1 = emon2.calcIrms(1480);  // Calculate Irms only
    double Irms2 = emon3.calcIrms(1480);  // Calculate Irms only
    String sensorReading = String(Vrms * emon1.calcIrms(1480));
    sensorReading += ",";
    sensorReading +=  String(Vrms * emon2.calcIrms(1480));
    sensorReading += ",";
    sensorReading +=  String(Vrms * emon3.calcIrms(1480));
    sendToEmonCMS(node, sensorReading);
    setAdcDelay();
  }
}
