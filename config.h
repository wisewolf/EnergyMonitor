#ifndef CONFIG_H_
#define CONFIG_H_
/*
   Configuration file
*/

// Configure your CT Phase channel assignment
// Assume PHASE_R for monophasic installations
#define PHASE_R 0
#define PHASE_S 1
#define PHASE_T 2

// Configure optional Voltage sensor for power factor calculation
#define PHASE_VOLT_R 3
#define PHASE_VOLT_S 4
#define PHASE_VOLT_T 5

// Configure optional CT sensors
#define PHASE_A 3
#define PHASE_B 4
#define PHASE_C 5
#define PHASE_D 6
#define PHASE_D 7

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
