

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

#include <SPIFFS.h>



SettingManager::SettingManager(unsigned char pinLed) : BaseSettingManager(pinLed){

}



int sort_desc(const void *cmp1, const void *cmp2)
{
  // Need to cast the void * to int *
  Page *a = (Page *)cmp1;
  Page *b = (Page *)cmp2;  
  // The comparison
  return a->hourMinuteConverted - b->hourMinuteConverted;
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
    } else {
      DEBUGLOGF("Config file SART READING [%s]",error.c_str());
    }
    file.close();
  } else {
      DEBUGLOG("Config file does not exist");
  }

  qsort(customPage, SettingManager::nbCustomPages, sizeof(customPage[0]), sort_desc);
  for (uint8_t iPage =0; iPage < SettingManager::nbCustomPages; iPage++) {
    DEBUGLOGF("[%i] Page[%s] [%s]\n",iPage, customPage[iPage].name.c_str(), customPage[iPage].hourMinute.c_str());
    customPage[iPage].nbElement = 6;
  }
  switchOff();
  return m_iEEprom;
}

/*
void SettingManager::writePage(Page *page) {
	char tmpText[20];
	page->name.toCharArray(tmpText, 20);
	writeEEPROM(tmpText);
	writeEEPROM(page->id);
	writeEEPROM(page->nbElement);

	for (uint8_t iElt =0; iElt < page->nbElement; iElt++) {
		writeEEPROM(page->element[iElt].type);
		writeEEPROM(page->element[iElt].font);
		writeEEPROM(page->element[iElt].active);
		writeEEPROM(page->element[iElt].id);
		writeEEPROM(page->element[iElt].x);
		writeEEPROM(page->element[iElt].y);
		//writeEEPROM(page->xDec);
		writeEEPROM(page->element[iElt].red);
		writeEEPROM(page->element[iElt].green);
		writeEEPROM(page->element[iElt].blue);

		page->element[iElt].txt.toCharArray(tmpText, 20);
		writeEEPROM(tmpText);
	}
}*/

/*

void SettingManager::readPage(Page *page) {
	char tmpText[21];
	readEEPROM(tmpText,20);
	page->name = String(tmpText);
	page->id = readEEPROM();
	page->nbElement = readEEPROM();

	for (uint8_t iElt =0; iElt < page->nbElement; iElt++) {

		page->element[iElt].type = (Element::OBJECT_TYPE)readEEPROM();
		page->element[iElt].font = (Element::FONT_TYPE)readEEPROM();
		page->element[iElt].active = readEEPROM();
		page->element[iElt].id = readEEPROM();
		page->element[iElt].x = readEEPROM();
		page->element[iElt].y = readEEPROM();
		//writeEEPROM(page->xDec);
		page->element[iElt].red = readEEPROM();
		page->element[iElt].green = readEEPROM();
		page->element[iElt].blue = readEEPROM();
		readEEPROM(tmpText,20);
		page->element[iElt].txt= String(tmpText);
	}
}
*/

unsigned char SettingManager::writeData(){
  mpPages->stopTimer();

  BaseSettingManager::writeData();
  switchOn();
/*
  //active
  writeEEPROM(displayedPage);
  writeEEPROM(displayedMode);
  writeEEPROM(displayedFrequence);

  for (uint8_t p=0 ;p<nbCustomPages ;p++) {
	  writePage(&customPage[p]);
  }

  DEBUGLOGF("SettingManager::writeData() [%d)]\n",m_iEEprom );
  EEPROM.commit();
  //portEXIT_CRITICAL_ISR(&wtimerMux);
  DEBUGLOGF("SettingManager::writeData() Commit [%d)]\n",m_iEEprom );
  setStatus( m_iEEprom, "written");
*/

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

  mpPages->startTimer();
  switchOff();
  return m_iEEprom;
}

