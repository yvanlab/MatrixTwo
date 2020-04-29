

#ifndef dictons_h
#define dictons_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"
#else
#include "Arduino.h"
#include "WProgram.h"

#endif
#include "SPIFFS.h"
#include <SD.h>


class Dictons
{
  public:
    Dictons() {};
    
    String getDictons() {
        return m_currentDicton;
    }

    void findNewDicton(uint8_t numDay) {
        uint8_t nbLine = 0;
        char cfgFileName[] =  "/dictons.txt";
        if (SPIFFS.exists(cfgFileName)){
            File file = SPIFFS.open(cfgFileName, FILE_READ);
            while (file.available() && nbLine < numDay ) {
                m_currentDicton = file.readStringUntil('\n');
                nbLine++;
            }        
            file.close();
        } else {
            DEBUGLOGF("%s file does not exist\n", cfgFileName);
        }
        DEBUGLOGF("Dictons [%d][%s]\n",numDay, m_currentDicton.c_str());
        return;
    };


  private:
    String m_currentDicton;
};


#endif
