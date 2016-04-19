/*
  Example code to get a MCP3*08 running with an ESP8266
  for DiY energy monitoring solutions
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"
#include "MCPEmon/MCPEmon.h"

const char* ssid = "...";
const char* password = "...";
const int   Vrms = 230;

// Setup Energy Monitor object
MCPEmon adc(SPICLOCK, DATAOUT, DATAIN, SELPIN);

// ADC read time "delays"
unsigned long adcTime;
unsigned long adcDelay = 5000;

void sendToCloud(String data) {
  WiFiClient client;
  if (!client.connect(HOST, HOSTPORT)) {
    return;
  }

  // We now create a URI for the request
  String url = "/input/post.json?node=";
  url += NODEID;
  url += "&apikey=";
  url += NODEAPIKEY;
  url += "&csv=";
  url += data;

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + HOST + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server
  while (client.available()) {
    String line = client.readStringUntil('\r');
  }
}

void setup() {
  adcTime = millis() + adcDelay;           // Set ADC read delay
}

void loop() {
  String sensorReading = "";
  double Irms0, Irms1, Irms2;

  if (millis() >= adcTime) {
    // reading a triphase installation
    Irms0 = adc.calcIrms(1480,PHASE_R);  // Calculate Irms only
    Irms1 = adc.calcIrms(1480,PHASE_S);  // Calculate Irms only
    Irms2 = adc.calcIrms(1480,PHASE_T);  // Calculate Irms only
    
    // compose string with data
    sensorReading  = String(Vrms * Irms0);
    sensorReading += ",";
    sensorReading += String(Vrms * Irms1);
    sensorReading += ",";
    sensorReading += String(Vrms * Irms2);
    
    // send it
    sendToCloud(sensorReading);
    
    // reset next run
    adcTime = millis() + adcDelay;         // Set ADC read delay
  }
}
