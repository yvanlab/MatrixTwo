

#ifndef grovestreamsManager_h
#define grovestreamsManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#ifdef ESP32
#include <HTTPClient.h>
#include <wifi.h>

#else
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif


#include "BaseManager.h"

//#include <ThingSpeak.h>

//enum IOT_GROUP_TYPE {METEO,COURANT,PRESENCE}

#define TEMP_ID "Meteo"
#define TEMP_SEJOUR "Sejour"
#define TEMP_GARAGE "Garage"
#define TEMP_VMC    "VMC"
#define TEMP_EXT    "Ext"
#define TEMP_DRESSING "Dressing"
#define TEMP_PRESSION "Pression"

#define COURANT_ID "Courant"
#define COURANT_GENERAL "General"
#define COURANT_LIGNE_1 "lgn_1"
#define COURANT_LIGNE_2 "lgn_2"
#define COURANT_LIGNE_3 "lgn_3"

#define PRESENCE_ID "Presence"
#define PRESENCE_SEJOUR_BAR "Bar"
#define PRESENCE_SEJOUR_COULOIR "Couloir"
#define PRESENCE_MATRIX_2 "Matrix2"

#define API_KEY "8edfca1e-9646-3091-8f24-696d0e923634"
  

//http://grovestreams.com/api/feed?compId=comp1&temp=6&data=1&api_key=8edfca1e-9646-3091-8f24-696d0e923634

//Meteo
//http://grovestreams.com/api/feed?compId=Meteo&Sejour=1&Garage=2&VMC=3&EXT=4&Dressing=5&Pression=6&api_key=8edfca1e-9646-3091-8f24-696d0e923634

//Courant
//http://grovestreams.com/api/feed?compId=Courant&General=1&lgn_1=2&lgn_2=3&lgn_3=4&api_key=8edfca1e-9646-3091-8f24-696d0e923634

// Presence
//http://grovestreams.com/api/feed?compId=Presence&Bar=1&Couloir=2&Matrix2=3&api_key=8edfca1e-9646-3091-8f24-696d0e923634



typedef struct
{
    String                  Name;
    String                  Value;
} MyiotVariable;

class grovestreamsManager : public BaseManager
{

  public:

    grovestreamsManager( unsigned char pinLed);

    int sendIoT(char *sGrpIDName);

    String toString(boolean bJson);
    void addVariable(String name, String value);

    String getClassName(){return "grovestreamsManager";}
  private:
    MyiotVariable m_variables[8];
    unsigned char m_index;

  };

#endif
