

#ifndef BMPManager_h
#define BMPManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#include <Wire.h>
#include <Sodaq_BMP085.h>
#include "BaseManager.h"

#define BPM_TEMPS_BEAU     1 //:   > 1020               :  Beau ou assez beau. Journées chaudes, nuits fraîches.
#define BPM_TEMPS_ORAGEUX  2 //: De 1013 à 1020 hPa	  :  Assez beau ou ondées orageuses. Journées chaudes, nuits un peu fraîches.
#define BPM_TEMPS_PLUIE    3 //: De 1006 à 1013 hPa   :	 Pluie ou averses avec un peu de vent. Temps doux.
#define BPM_TEMPS_TEMPETE  4 //:  < 1006 hPa	          :  Pluie et vent assez fort. Températures douces.


class BMPManager : public Sodaq_BMP085, public BaseManager
{

  public:
    BMPManager(unsigned char SDA, unsigned char SCL,unsigned char pinLed);

    String toString(boolean bJson);
    String getClassName(){return "BMPManager";}

    int32_t mesurePressure(void);
    float   mesureTemperature(void);
    float   getTemperature(void);
    int32_t getPressure(void);
    uint8_t getWeatherTrend(float fPression);
    float getLastTemperature(){return m_Temperature.m_value;};
    int32_t getLastPressure(){return m_Pression.m_value;};

    capteurValue  m_Pression;
    capteurValue  m_Temperature;
};

#endif
