

#ifndef BMPManagerV2_h
#define BMPManagerV2_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
//#include <DHT.h>
#include "BMPPression.h"
#include "BMPTemperature.h"
//#include "baseManager.h"

//#define MCPOC_MOCK 1



class BMPManagerV2 : public SensorBase
{

  public:

    BMPPression     m_Pression;
    BMPTemperature  m_Temperature;
    Adafruit_BMP085 m_bmp;

    BMPManagerV2(uint8_t pinLed) : SensorBase(pinLed) {

      m_Pression.setup(pinLed,&m_bmp);//  = new DHTHumidity
      m_Temperature.setup(pinLed,&m_bmp);
	  #if !defined(MCPOC_MOCK)
      m_bmp.begin();
     #endif

    };

    String toString(boolean bJson)  {
      String res;
      if (bJson == JSON_TEXT) {
    	/*  DEBUGLOG("\"pression\":{"+m_Pression.toString(bJson));
    	  DEBUGLOG(",\"forcast\":\""+ String(m_Pression.getWeatherTrend()));
    	  DEBUGLOG("\"Btemperature\":{"+m_Temperature.toString(bJson));*/
        res = "\"pression\":{"+m_Pression.toString(bJson) + ",\"forcast\":\""+ String(m_Pression.getWeatherTrend())+"\"}," +
            "\"Btemperature\":{"+m_Temperature.toString(bJson)+"}";
      } else {
        res = "pression : "+m_Pression.toString(bJson)+" , temperature : "+m_Temperature.toString(bJson);
      }
      return res;
    };

    float mesure() {
      m_Pression.mesure();
      m_Temperature.mesure();
      setStatus( m_Temperature.getStatusCode(),m_Temperature.getStatusDescription());
      return 0.0;
    };

    void clear() {
        m_Pression.clear();
        m_Temperature.clear();
    }

    void set() {
        m_Pression.set();
        m_Temperature.set();
    }


    BMPPression* getPressionSensor() {return &m_Pression;};
    BMPTemperature* getTemperatureSensor() {return &m_Temperature;};

    String getClassName(){return "BMPManagerV2";}

  //private:




    //int8_t read();

};

#endif
