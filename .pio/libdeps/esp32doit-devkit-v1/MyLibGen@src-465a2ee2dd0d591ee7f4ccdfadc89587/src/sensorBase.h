

#ifndef sensorBase_h
#define sensorBase_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#include <BaseManager.h>

class SensorBase : public BaseManager
{

  public:

    SensorBase():BaseManager(){;};
    SensorBase(uint8_t pinLed);

    String toString(boolean bJson);
    virtual float mesure() = 0;
    float getValue();
    float getTrend()        {return m_capteurValue.m_trend;};
    float getLastValue()    {return m_capteurValue.m_value;};
    float getAverage();
    float getLastMinValue() {return m_capteurValue.m_minValue;};
    float getLastMaxValue() {return m_capteurValue.m_maxValue;};
    void  clear()           {m_capteurValue.clear();};
    void  set()           {m_capteurValue.set();};
    String getClassName()   {return "SensorBase";}
  protected:
    capteurValue  m_capteurValue;

};

#endif
