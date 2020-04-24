

#include "periferic.h"
#include <WiFiClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
  
  
  

Periferic::Periferic(unsigned char pinLed) : BaseManager(pinLed){

}


 DeserializationError Periferic::collectJson(String urlService,JsonDocument *doc,  JsonDocument *filter)
 {
  DeserializationError error(DeserializationError::NotSupported);
  if (!WiFi.isConnected()) return error;
  HTTPClient http;  //Declare an object of class HTTPClient
  DEBUGLOG(urlService);  //Specify request destination
  http.setTimeout(2000);
  http.setConnectTimeout(2000);
  client.setTimeout(2000);
  http.begin(client, urlService);  //Specify request destination
  //DEBUGLOG("http.begin");
  int httpCode = http.GET();                                                                  //Send the request
  //DEBUGLOGF("http.GET %d\n",httpCode);

  if (httpCode > 0) { //Check the returning code
    //DEBUGLOG(http.getString());
    //DEBUGLOG("Before deserializeJson");
    error = deserializeJson(*doc,client,DeserializationOption::Filter(*filter));
    //DEBUGLOG("Before serializeJsonPretty");
    //serializeJsonPretty(*doc, Serial);
    //String result = http.getString();   //Get the request response payload
    
  } else 
      DEBUGLOG("http error");

   //client.stop();
  http.end();   //Close connection
  
  //DEBUGLOG(httpCode);
  //DEBUGLOG(error.c_str());
  return error;
}


void Periferic::retrievePeriphericInfo() {

  DEBUGLOGF("Free A [%d]\n", ESP.getFreeHeap());
  DynamicJsonDocument doc(1000);
  StaticJsonDocument<200> filter;
  DeserializationError error;

  filter[F("courant1")] = true;
  filter[F("temperature")] = true;

  error = collectJson("http://"+String(CURRENT_GARAGE_IP_PROD)+"/status",&doc, &filter);
  if (!error) {
    m_InstantCurrentGarage = (doc[F("courant1")][F("value")]).as<float>()/1000.0;
    m_TemperartureGarage   = doc[F("temperature")][F("value")];
  } else {
    m_InstantCurrentGarage = 0.0;
    m_TemperartureGarage   = 0.0;
    DEBUGLOGF("retrievePeriphericInfo GARAGE [%s] [%s]\n",CURRENT_GARAGE_IP_PROD, error.c_str()); 
  }
  //DEBUGLOGF("Free B [%d]\n", ESP.getFreeHeap());

  filter.clear();
  filter[F("EXT")][F("Btemperature")] = true;
  error = collectJson("http://"+String(VMC_METEO_IP_PROD)+"/status",&doc, &filter);
  if (!error) {
    m_ExtTemp = doc[F("EXT")][F("Btemperature")][F("value")];
  } else {
    m_ExtTemp = 0.0;
    DEBUGLOGF("retrievePeriphericInfo VMC [%s] [%s]\n",VMC_METEO_IP_PROD, error.c_str()); 
  }
  //DEBUGLOGF("Free C [%d]\n", ESP.getFreeHeap());
}

