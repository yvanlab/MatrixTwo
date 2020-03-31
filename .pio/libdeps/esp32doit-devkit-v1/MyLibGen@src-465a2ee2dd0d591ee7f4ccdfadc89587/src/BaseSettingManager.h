

#ifndef BaseSettingManager_h
#define BaseSettingManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif


#include "BaseManager.h"
#include <EEPROM.h>

class BaseSettingManager : public BaseManager
{
  public:
    char m_ssid[32];
    char m_password[64];
    char m_privateKey[64];
    char m_publicKey[64];

    BaseSettingManager(unsigned char pinLed);
    String toString(boolean bJson);
    virtual unsigned char readData();
    virtual unsigned char writeData();
    virtual unsigned char clearData();
    String getClassName(){return "BaseSettingManager";}
  protected:
    uint8_t writeEEPROM(int32_t value);
    //uint8_t writeEEPROM(int16_t value);
    uint8_t writeEEPROM(char *str);
    char* readEEPROM(char *str,uint8_t maxChar);
    //int16_t readEEPROM();
    int32_t readEEPROM();
    uint8_t m_version = 0;
  protected:
    int16_t m_iEEprom = 0;


};

#endif
