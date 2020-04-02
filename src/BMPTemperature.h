

#ifndef BMPTemperature_h
#define BMPTemperature_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
//#include "baseManager.h"

#include "BMPBase.h"
//#include <sensorBase.h>
#include <Sodaq_BMP085.h>

class BMPTemperature : public BMPBase
{

  public:

    BMPTemperature() : BMPBase() {;};
    BMPTemperature(uint8_t pinLed, Sodaq_BMP085 *bmp):BMPBase(pinLed,bmp){
    ;
    };

    float mesure(){
      switchOn();
      float temp;
      #if defined(MCPOC_MOCK)
      temp = random(40);
      setStatus( true,"Mocking");
      #else
      temp = m_BMP->readTemperature();
      setStatus( true,"OK");
      #endif
      //DEBUGLOGF("mesure T %f\n",temp);
      m_capteurValue.mesure(temp);

      switchOff();
      return temp;
    }

    String getClassName(){return "BMPTemperature";}


};

#endif
