

#ifndef BMPPression_h
#define BMPPression_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
//#include "baseManager.h"

#include "BMPBase.h"

//#include <DHT.h>
//#include <sensorBase.h>
//#include <Wire.h>
#include <Adafruit_BMP085.h>
#define BPM_TEMPS_BEAU     1 //:   > 1020               :  Beau ou assez beau. Journées chaudes, nuits fraîches.
#define BPM_TEMPS_ORAGEUX  2 //: De 1013 à 1020 hPa	  :  Assez beau ou ondées orageuses. Journées chaudes, nuits un peu fraîches.
#define BPM_TEMPS_PLUIE    3 //: De 1006 à 1013 hPa   :	 Pluie ou averses avec un peu de vent. Temps doux.
#define BPM_TEMPS_TEMPETE  4 //:  < 1006 hPa	          :  Pluie et vent assez fort. Températures douces.




class BMPPression : public BMPBase
{

  public:

    BMPPression() : BMPBase() {;};
    BMPPression(uint8_t pinLed, Adafruit_BMP085 *bmp):BMPBase(pinLed,bmp){
    ;
    };

    float mesure(){

    	switchOn();

      float pressure;
      #if defined(MCPOC_MOCK)
      pressure = random(100000);
      setStatus( true,"Mocking");
      #else
      pressure = m_BMP->readPressure();
      setStatus( true,"OK");
      #endif
      m_capteurValue.mesure(pressure);
      //DEBUGLOGF("mesure P %f\n",pressure);

      switchOff();
      return pressure;
    }

    uint8_t getWeatherTrend() {
      float fPression = getLastValue();
      if  (fPression>102000) return BPM_TEMPS_BEAU;
      if  (fPression>101300 && fPression<102000) return BPM_TEMPS_ORAGEUX;
      if  (fPression>100600 && fPression<101300) return BPM_TEMPS_PLUIE;
      if  (fPression<100600 ) return BPM_TEMPS_TEMPETE;
      return BPM_TEMPS_BEAU;
    }

    String getClassName(){return "BMPPression";}


};

#endif
