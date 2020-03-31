

#ifndef HourManager_h
#define HourManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include <NtpClientLib.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiType.h>
#endif

#ifdef ESP32
#include <WiFi.h>
//#include <functional>
#endif


#include <Wire.h>
//#include <RTClib.h>
#include "BaseManager.h"

class HourManager : /*public NTPClient, */public BaseManager
{

  public:
    const unsigned char NTP_PACKET_SIZE = 48;
    const char* HM_NTP_SERVER_NAME = "time.nist.gov";

    HourManager(unsigned int localPort, unsigned char pinLed);
    bool begin(String ntpServerName, int timeOffset, bool daylight);
    String toDTString(boolean bJson);
    String toUTString();

    boolean  isNextDay();
    boolean  isNextHour();
    boolean  isNextMinute();
    boolean  isNight() {
        	DEBUGLOGF("hour : %d\n",hour());
        	return hour()>18 || hour()<05;
        }

        boolean  isMorning() {
        	return hour()>05 || hour()<12;
        }

    String getClassName(){return "HourManager";}
  private:

    unsigned int m_localPort = 2390;      // local port to listen for UDP packets
    void NTPsyncEvent(NTPSyncEvent_t ntpEvent);

    int8_t m_currentDay=-1;
     int8_t m_currentHour=-1;
     int8_t m_currentMinute=-1;

     bool m_hourSynchronised = false;

};

#endif
