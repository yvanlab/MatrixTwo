
#include "sensorBase.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



SensorBase::SensorBase(uint8_t pinLed)
  : BaseManager(pinLed) {

}

String SensorBase::toString(boolean bJson) {
  //return F("To be done");
  if (bJson==JSON_TEXT) {
	/*  DEBUGLOG("\"value\":\""+String (getLastValue()));
  DEBUGLOG("\", \"trend\":\"" + String(getTrend()));
  DEBUGLOG("\", \"minValue\":\"" + String(getLastMinValue()));
  DEBUGLOG("\", \"maxValue\":\"" + String(getLastMaxValue()));
return "";*/
    return  "\"value\":\""+String (getLastValue()) +
            "\", \"trend\":\"" + String(getTrend())+
            "\", \"minValue\":\"" + String(getLastMinValue())+
            "\", \"maxValue\":\"" + String(getLastMaxValue())+
            "\"";
  }else {
    return "Measure  average[" + String (getAverage()) + "] - sampling["+String (m_capteurValue.m_nbreMeasure) + "] - Cumul[" + String (m_capteurValue.m_cumulValue) + "]";
  }
}

float SensorBase::getAverage() {
  if (m_capteurValue.m_nbreMeasure != 0)
    return m_capteurValue.m_cumulValue/m_capteurValue.m_nbreMeasure;
  else
    return 0.0;
}
/*float SensorBase::mesure(){
  // read during 2 periods 50 Hz :
  switchOn();

  #ifdef MCPOC_MOCK
  setStatus(0, "mocked");
  #else
  setStatus(0, "ok");
  #endif

  switchOff();
  return 0;
}*/

/*unsigned long int ElectricManager::readCumulCurrent() {
  m_globalIntervalValue += readCurrent();
  m_globalNbreValue++;

}*/

float SensorBase::getValue(){
  if (m_capteurValue.m_nbreMeasure == 0) mesure();
  m_capteurValue.set();
  return m_capteurValue.m_value;
}
