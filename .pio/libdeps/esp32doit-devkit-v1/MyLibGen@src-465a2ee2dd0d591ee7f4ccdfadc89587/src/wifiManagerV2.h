

#ifndef WifiManager_h
#define WifiManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif


#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include <SPIFFS.h>
#include <Update.h>
//#include <ESP32HTTPUpdate.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FS.h>


#ifdef OTA_FOR_ATOM
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

extern "C" {
  #include "user_interface.h"
}
#endif

#include <HourManager.h>
#include <BaseManager.h>
#include <BaseSettingManager.h>

class WifiManager  : public BaseManager
{
  public:

    WifiManager(const uint8_t pinLed,  BaseSettingManager *_smManager);
    wl_status_t begin(IPAddress ip, const char *MODULE_NAME, const char *MODULE_MDNS, const char *MODULE_MDNS_AP);

    HourManager *getHourManager() {return _hrManager;} ;
#ifdef ESP8266
    ESP8266WebServer *getServer(){return _server;};
#endif
#ifdef ESP32
    WebServer *getServer(){return _server;};
#endif


    void handleClient();

    String toString(boolean bJson);
    //unsigned char connectSSID();
    wl_status_t connectSSID(char *ssid, char *pass, IPAddress ip, const char *MDNS_LABEL);
    unsigned char connectAP(const char *MDNS_LABEL);

    String getClassName(){return "WifiManager";}

    void restartESP();
    void displayCredentialCollection();
    void setCredential();
    void clearMemory();
    void wifiReset();
#ifdef ESP32
    void initHttpUpdater();
    void httpUpdaterPage();
#endif    
    bool loadFromSpiffs(String path);

    #ifdef OTA_FOR_ATOM
    void setOTAForAtom();
    #endif

  private:
    wl_status_t connecting(char *ssid, char *pass);
#ifdef ESP8266
    ESP8266WebServer        *_server;
#endif
#ifdef ESP32
    WebServer        *_server;
#endif

    HourManager             *_hrManager;
#ifdef ESP8266
    ESP8266HTTPUpdateServer _httpUpdater;
#endif
#ifdef ESP32
    //ESP32HTTPUpdate _httpUpdater;
#endif

    BaseSettingManager      *_basesmManager;


};

#endif
