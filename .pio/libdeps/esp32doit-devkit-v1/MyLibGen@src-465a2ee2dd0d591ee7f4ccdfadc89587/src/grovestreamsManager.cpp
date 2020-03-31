
#include "grovestreamsManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "BaseManager.h"


grovestreamsManager::grovestreamsManager( unsigned char pinLed) : BaseManager(pinLed)  {
  m_index  = 0;
}

void grovestreamsManager::addVariable(String name, String value) {
  m_variables[m_index].Name=name;
  m_variables[m_index].Value=value;
  m_index++;
}

//http://grovestreams.com/api/feed?compId=Meteo&Sejour=1&Garage=2&VMC=3&EXT=4&Dressing=5&Pression=6&api_key=8edfca1e-9646-3091-8f24-696d0e923634

int grovestreamsManager::sendIoT(char *sGrpIDName) {
  switchOn();

  String sURL = "http://grovestreams.com/api/feed?api_key=" + String(API_KEY);
  sURL  += "&compId=" + String(sGrpIDName);
  uint8_t i = 0;
  while (i < m_index) {
    sURL  += "&" + m_variables[i].Name + "=" +  m_variables[i].Value;
    i++;
  }

  m_index = 0;
  DEBUGLOGF("URL:%s\n",sURL.c_str() );

  WiFiClient client;
  HTTPClient http;

  http.begin(client,sURL );
  int res = http.PUT("");
  DEBUGLOGF("http.PUT():%d\n",res );
  if (res != HTTP_CODE_OK )
    setStatus(res, F("fail to connect grovestreams"));
  else
    setStatus(res, F("Ok"));
  switchOff();
  return res;
}

String grovestreamsManager::toString(boolean bJson = false) {
  String res = "variables :";
  unsigned char i=0;
  while (i < m_index) {
    res += "\""+String(m_variables[i].Name) + "\" : \"" + String(m_variables[i].Value+"\"");
    i++;
  }
  return res;
}
