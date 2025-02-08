
#include "BMPManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//#include <DHT.h>
//http://leguidemeteo.com/prevoir-le-temps-a-partir-de-son-barometre/

// BPM_TEMPS_BEAU     :   > 1020               :  Beau ou assez beau. Journées chaudes, nuits fraîches.
// BPM_TEMPS_ORAGEUX  : De 1013 à 1020 hPa	  :  Assez beau ou ondées orageuses. Journées chaudes, nuits un peu fraîches.
// BPM_TEMPS_PLUIE    : De 1006 à 1013 hPa   :	 Pluie ou averses avec un peu de vent. Temps doux.
// BPM_TEMPS_TEMPETE  :  < 1006 hPa	          :  Pluie et vent assez fort. Températures douces.


BMPManager::BMPManager(unsigned char SDA, unsigned char SCL,unsigned char pinLed)
  :Adafruit_BMP085() , BaseManager(pinLed) {
    setStatus( true,"OK");
}

uint8_t BMPManager::getWeatherTrend(float fPression) {
  if  (fPression>102000) return BPM_TEMPS_BEAU;
  if  (fPression>101300 && fPression<102000) return BPM_TEMPS_ORAGEUX;
  if  (fPression>100600 && fPression<101300) return BPM_TEMPS_PLUIE;
  if  (fPression<100600 ) return BPM_TEMPS_TEMPETE;
  return BPM_TEMPS_BEAU;
};

String BMPManager::toString(boolean bJson = STD_TEXT) {
    if (bJson==JSON_TEXT)
    return "\"bmpTemp\" : \""+String (getLastTemperature()) +
        "\", \"bmpMaxTemp\" : \""+String(m_Temperature.m_maxValue) +
        "\", \"bmpMinTemp\" : \""+String(m_Temperature.m_minValue) +
        "\", \"bmpTempTrend\" : \"" + String(m_Temperature.m_trend)+
        "\", \"bmpPressTrend\" : \"" + String(m_Pression.m_trend)+
        "\", \"bmpWeatherForcast\" : \"" + String(getWeatherTrend(getLastPressure()))+
        "\", \"bmpPress\" : \"" + String(getLastPressure())+"\"";
  else
    return "Temperature["+String (getLastTemperature()) + "]["+String(m_Temperature.m_trend)+"] - Pressure[" + String(getLastPressure())+ "]["+String(m_Pression.m_trend)+"]";
}


float BMPManager::mesureTemperature() {
  switchOn();
  float temp;
  #if defined(MCPOC_MOCK)
  temp = random(40);
  setStatus( true,"Mocking");
  #else
  temp = Adafruit_BMP085::readTemperature();
  setStatus( true,"OK");
  #endif
  m_Temperature.mesure(temp);

  switchOff();
  return temp;
}

float BMPManager::getTemperature() {
  if (m_Temperature.m_nbreMeasure == 0) mesureTemperature();
  m_Temperature.set();
  return m_Temperature.m_value;
}

int32_t BMPManager::mesurePressure(){
  switchOn();
  float pressure;
  #if defined(MCPOC_MOCK)
  pressure = random(100000);
  setStatus( true,"Mocking");
  #else
  pressure = Adafruit_BMP085::readPressure();
  setStatus( true,"OK");
  #endif
  m_Pression.mesure(pressure);

  switchOff();
  return pressure;
}

int32_t BMPManager::getPressure() {
  if (m_Pression.m_nbreMeasure == 0) mesurePressure();
  m_Pression.set();
  return m_Pression.m_value;
}
