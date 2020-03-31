

#ifndef thingSpeakManager_h
#define thingSpeakManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#ifdef ESP32
#include <wifi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <WiFiClient.h>
#include "BaseManager.h"

#include <ThingSpeak.h>


typedef struct
{
    uint8_t               Index;
    //String                  Name;
    String                  Value;
} iotVariable;

class thingSpeakManager : public BaseManager
{

  public:

    thingSpeakManager( unsigned char pinLed);

    int sendIoT(String privateKey, String publicKey);

    String toString(boolean bJson);
    void addVariable(uint8_t index, String value);

    String getClassName(){return "thingSpeakManager";}
  private:
    iotVariable m_variables[8];
    unsigned char m_index;
    WiFiClient m_client;
  };

#endif
