
#ifndef FlashLed_h
#define FlashLed_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

//#include <BaseManager.h>

//FlashLed flLed(pinLed);

#define FLASH_STOP  0
#define FLASH_ON    1
#define FLASH_25ms  2
#define FLASH_250ms 3
#define FLASH_1s    4

class FlashLed
{
  public:
    FlashLed(uint8_t pinLed) {
      m_pinLed        = pinLed;
    } ;

    void startFlashLed(uint16_t duration, uint8_t mode) {
      m_mode = mode;
      m_duration = duration;
      m_TimerStart = millis();
      //DEBUGLOGF("startFlashLed mode %d\n ",m_mode);
    }

    void  flashCallback() {
      //DEBUGLOG("In callback ");
      //DEBUGLOGF("flashCallback mode %d\n ",m_mode);
      if (m_mode == FLASH_STOP) return;
      //DEBUGLOG("In callback ");
      if ((millis()-m_TimerStart) < m_duration) {
        switch (m_mode) {
          case FLASH_ON :
            digitalWrite ( m_pinLed, LOW );
            break;
          case FLASH_25ms :
            digitalWrite ( m_pinLed, mtTimer.is25MSFrequence() );
            break;
          case FLASH_250ms :
            digitalWrite ( m_pinLed, mtTimer.is250MSFrequence() );
            break;
          case FLASH_1s :
            digitalWrite ( m_pinLed, mtTimer.is1SFrequence() );
            break;
        }

      } else {
        digitalWrite ( m_pinLed, HIGH );
        m_mode = FLASH_STOP;
      }
    }


  public:
    uint8_t         m_mode = FLASH_STOP;
    uint16_t        m_duration = 0;
    uint8_t         m_pinLed;
    uint32_t        m_TimerStart = 0;
};

#endif
