
#ifndef main_h
#define main_h

#include <Arduino.h>


#include <WifiManagerV2.h>
#include <myTimer.h>
//#include <FlashLed.h>

//#include <ESP8266HTTPClient.h>
//#include <SPI.h>

#include "SettingManager.h"

#include "networkUI.h"

//#include "thingSpeakManager.h"
#include "MatrixPages.h"
#include "BMPManagerV2.h"
#include <presenceHelper.h>
#include <thingSpeakManager.h>
/*#include "periferic.h"
#include "SensorInterface.h"*/





#define PIN_LED 2

#define MODULE_NAME     LA_MATRIX2_NAME
#define MODULE_MDNS     LA_MATRIX2_MDNS
#define MODULE_MDNS_AP  LA_MATRIX2_MDNS_AP
#define MODULE_IP       LA_MATRIX2_IP



#define LAPORTE_OUVERTURE_LABEL  1 //"current"



extern SettingManager      *smManager;
extern WifiManager         *wfManager;
extern BMPManagerV2		   *bmpMesure;
extern MatrixPages         *mpPages;


#ifdef MCPOC_TELNET
//extern RemoteDebug          Debug;
#endif

#ifdef ESP32
extern portMUX_TYPE 		wtimerMux;// = portMUX_INITIALIZER_UNLOCKED;
#endif


#endif
