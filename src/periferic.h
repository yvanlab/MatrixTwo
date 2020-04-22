

#ifndef periferic_h
#define periferic_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"
//#include "WProgram.h"
#else
#include "Arduino.h"
#include "WProgram.h"

#endif
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "baseManager.h"

class Periferic : public BaseManager
{
  public:
    Periferic(unsigned char pinLed);
    DeserializationError collectJson(String urlService,JsonDocument *doc,  JsonDocument *filter );
    
    //String collectJson(String ipAddress);
    //String buildPerifericOutput(String perifericName, uint8_t statusCode);
    void retrievePeriphericInfo();

    float getExtTemp()     {return m_ExtTemp;}

    float getInstantCurrentGarage()   {return m_InstantCurrentGarage;}
    float getITemperartureGarage()    {return m_TemperartureGarage;}

    //String getDaySaintName()    {return getKeyValue(m_saintJson,"name");}

    String getClassName()     {return "Periferic";}

    // current
    float  m_InstantCurrentGarage = 0.0;
    float  m_TemperartureGarage = 0.0;
    
    // meteo
    float m_ExtTemp = 0.0;
    

/*    String m_ExtTempMax;
    String m_ExtTempMin;*/

  private:
/*    String getBlock(String jSon, String key);
    String getValue(String jSon, String key);
    String getKeyValue(String jSon, String key);*/
    
    WiFiClient client;


};


#endif
