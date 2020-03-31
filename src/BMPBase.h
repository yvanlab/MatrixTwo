

#ifndef BMPBase_h
#define BMPBase_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
//#include "BaseManager.h"

#include "SensorBase.h"

#include <Wire.h>
#include <Sodaq_BMP085.h>

class BMPBase : public SensorBase
{

  public:
    Sodaq_BMP085 * m_BMP;

    BMPBase() : SensorBase() {;};
    BMPBase(uint8_t pinLed,Sodaq_BMP085 *bmp):SensorBase(pinLed){
      //m_pinDHT = pinLed;
      m_BMP = bmp;
    };

    void setup(uint8_t pinLed,Sodaq_BMP085 *bmp) {
      m_BMP = bmp;
      SensorBase::setup(pinLed);
    }


};

#endif
