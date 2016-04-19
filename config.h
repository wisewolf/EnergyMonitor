#ifndef CONFIG_H_
#define CONFIG_H_
/*
   Configuration file
*/

// global includes
#include <Arduino.h>

// Choose your ESP Module, (defaults to NODEMCU10)
//#define ESP01
//#define ESP12EQ
#define NODEMCU10

// Choose your ADC, (defaults to MCP3208)
//#define MCP3008
#define MCP3208

// Choose your cloud endpoint, (defaults to emoncms)

// DIY ITO CLOUD, Use whatever you need for your case but please configure at least host, apikey and deviceid
//#define DIYIOT
#if defined(DIYIOT)
#define HOST "127.0.0.1"
#define HOSTPORT 80
#define NODEAPIKEY "your_key_goes_here"
#define NODEID 1
#endif

// EmonCMS (emoncms.org)
#define EMONCMS
#if defined(EMONCMS)
#define HOST "emoncms.org"
#define HOSTPORT 80
#define NODEAPIKEY "your_key_goes_here"
#define NODEID 1
#endif


/*
   Do NOT change anything below these lines unless instructed to do so by the developers
*/

// pin definitions for NodeMCU 1.0
#if defined(NODEMCU10)
#define SELPIN   D8  //CS
#define DATAOUT  D7  //MOSI
#define DATAIN   D6  //MISO
#define SPICLOCK D5  //SCLK
#endif

// pin definitions for generic ESP-01 modules (8 pins)
#if defined(ESP01)
#define SELPIN   0  //CS
#define DATAOUT  1  //MOSI
#define DATAIN   3  //MISO
#define SPICLOCK 2  //SCLK
#endif

#endif /* CONFIG_H_ */
