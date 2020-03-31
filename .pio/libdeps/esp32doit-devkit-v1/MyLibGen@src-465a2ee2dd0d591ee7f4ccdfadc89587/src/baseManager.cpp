

#include "BaseManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

BaseManager::BaseManager(unsigned char pinLed){
  m_pinLed = pinLed;
  m_changed = true;
  if (m_pinLed!=0)
    pinMode ( m_pinLed, OUTPUT );
}

void BaseManager::switchOn(){
  if (m_pinLed!=0)
  #ifdef ESP32 
    digitalWrite ( m_pinLed, HIGH );
  #else
    digitalWrite ( m_pinLed, LOW );
  #endif
}
void BaseManager::switchOff(){
  if (m_pinLed!=0) 
  #ifdef ESP32 
    digitalWrite ( m_pinLed, LOW );
  #else
    digitalWrite ( m_pinLed, HIGH );
  #endif
}

/*
{
"nom" : "class name",
"code" : "10",
"description" : "blabla"
},
*/

String BaseManager::log(boolean bJson = STD_TEXT){
  if (bJson==JSON_TEXT)
    return "{\"nom\":\""+getClassName()+"\",\"code\":\"" + String(m_codeStatus) +"\",\"description\":\""+m_description+"\"}";
  else {
    return getClassName()+ "[" + String(m_codeStatus) + "]:" + m_description;
  }
}
