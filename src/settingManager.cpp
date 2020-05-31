

#include "settingManager.h"
#include "main.h"
#include "SPIFFS.h"
#include <SD.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef ESP32
extern portMUX_TYPE 		wtimerMux;// = portMUX_INITIALIZER_UNLOCKED;
#endif


SettingManager::SettingManager(unsigned char pinLed) : BaseSettingManager(pinLed){

}


	String SettingManager::toStringCfg(boolean bJson)
	{
		String ss;
		if (bJson == STD_TEXT)
			return BaseSettingManager::toString(bJson);
		ss = "\"displayedPage\":\"" + String(displayedPage) + "\",";
		ss += "\"displayedMode\":\"" + String(displayedMode) + "\",";
		ss += "\"displayedFreq\":\"" + String(displayedMode) + "\",";
		ss += "\"page\":[";
		for (uint8_t iPage = 0; iPage < nbCustomPages; iPage++)
		{
			ss += customPage[iPage].toString(JSON_TEXT);
			if (iPage != nbCustomPages - 1)
				ss = ss + ",";
		}
		ss = ss + "]";
		return ss;
	}

	String SettingManager::toString(boolean bJson)
	{
		String ss;
		if (bJson == STD_TEXT)
		{
			ss = BaseSettingManager::toString(bJson);
		}
		else
		{
			uint8_t iIndex = 0;
			ss += "\"lstBMP\":[";
			while (iIndex < 10 && lstBMP[iIndex] != NULL)
			{
				ss += "\"" + String(lstBMP[iIndex]).substring(1) + "\"";
				iIndex++;
				if (iIndex < 10 && lstBMP[iIndex] != NULL)
					ss += ",";
			}
			ss += "],";

			ss = ss + toStringCfg(bJson);
			ss += ",";
      ss = ss + customPrg.toString(bJson);
		}
		return ss;
	}

	Page *SettingManager::getPage(uint16_t id)
	{
		for (uint8_t iPage = 0; iPage < nbCustomPages; iPage++)
		{
			//DEBUGLOGF("customPage [%d/%d]\n",customPage[iPage].id,id);
			if (customPage[iPage].id == id)
			{
				return &customPage[iPage];
			}
		}
		return NULL;
	}



int sort_desc(const void *cmp1, const void *cmp2)
{
  // Need to cast the void * to int *
  Page *a = (Page *)cmp1;
  Page *b = (Page *)cmp2;  
  // The comparison
  //DEBUGLOGF("Page[%s][%s][%d],[%s][%s][%d]\n",a->name.c_str(),a->hourMinute.c_str(),a->hourMinuteConverted,  
  //b->name.c_str(),b->hourMinute.c_str(),b->hourMinuteConverted);

  if (a->hourMinuteConverted >  b->hourMinuteConverted) 
    return 1;
  if (a->hourMinuteConverted ==  b->hourMinuteConverted)   
    return 0;
  if (a->hourMinuteConverted <  b->hourMinuteConverted)     
    return -1;
/*
  if (b->hourMinuteConverted>=0) {
    return a->hourMinuteConverted - b->hourMinuteConverted;
  } else {
    return -1;
  }*/
}


unsigned char SettingManager::readData(){
  BaseSettingManager::readData();
  switchOn();

  for (uint8_t i=0; i<SettingManager::nbCustomPages; i++) {
    customPage[i].set(i+1,"empty"+String(i), 6);
  }


  // Read BMP List
  String str = "";
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  uint8_t iTab = 0;
  while (file) {
    str = file.name();
    if (str.indexOf(".bmp")>0 ) {
      lstBMP[iTab] = new char[str.length()+1];
      str.toCharArray(lstBMP[iTab],str.length()+1);
      iTab++;
    }    
    DEBUGLOGF("BMP File %s[%d]\n", str.c_str(),file.size());
    file.close();
    file = root.openNextFile();
  }
  // Read configuartion file
  char cfgFileName[] =  "/config.json";
  if (SPIFFS.exists(cfgFileName)){
    DynamicJsonDocument doc(25000);
    file = SPIFFS.open(cfgFileName, FILE_READ);
    DeserializationError error = deserializeJson(doc, file);
    if (!error) {
      displayedPage = doc[F("displayedPage")];
      displayedMode = doc[F("displayedMode")];
      displayedFrequence = doc[F("displayedFreq")];
      JsonArray lstPage = doc[F("page")];
		  for (uint8_t iPage =0; iPage < lstPage.size(); iPage++) {
			  customPage[iPage].fromJson(lstPage[iPage]);
		  }
      sortPages();
    } else {
      DEBUGLOGF("Config file SART READING [%s]",error.c_str());
    }
    file.close();
  } else {
      DEBUGLOG("Config file does not exist");
  }

  customPrg.readData();

  switchOff();
  return m_iEEprom;
}

void SettingManager::sortPages(){
  qsort(customPage, SettingManager::nbCustomPages, sizeof(customPage[0]), sort_desc);
  for (uint8_t iPage =0; iPage < SettingManager::nbCustomPages; iPage++) {
    DEBUGLOGF("[%i] Page[%s][%s][%d]\n",customPage[iPage].id, customPage[iPage].name.c_str(), customPage[iPage].hourMinute.c_str(),customPage[iPage].hourMinuteConverted );
    customPage[iPage].nbElement = 6;
  }
}

unsigned char SettingManager::writeData(){
  mpPages->stopTimer();

  BaseSettingManager::writeData();
  switchOn();

  //Write Spiff configuration
  String ss;
  ss = "{\"datetime\":{" + wfManager->getHourManager()->toDTString(JSON_TEXT) + "},";
  ss += toStringCfg(JSON_TEXT)+"}";
  File cfgFile = SPIFFS.open("/config.json", FILE_WRITE);
  if (cfgFile.print(ss) == ss.length() ) {
    DEBUGLOGF("Configuration file saved");
  } else {
    DEBUGLOGF("Configuration PROBLEM");
  };
  cfgFile.close();

  customPrg.writeData();

  mpPages->startTimer();
  switchOff();
  return m_iEEprom;
}

