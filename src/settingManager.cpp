

#include "settingManager.h"
#include "main.h"
#include "SPIFFS.h"

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

#include <SD.h>

unsigned char SettingManager::readData(){
  BaseSettingManager::readData();
  switchOn();
/*  displayedPage = readEEPROM();
  displayedMode = readEEPROM();
  displayedFrequence = readEEPROM();*/


  customPage[0].set(CUSTOM_0_PAGE_ID, "cust1", 6);
  customPage[1].set(CUSTOM_1_PAGE_ID, "cust2", 6);
  customPage[2].set(CUSTOM_2_PAGE_ID, "cust3", 6);
  /*Page 	tmpPage[3];
  readPage(&tmpPage[0]);
  readPage(&tmpPage[1]);
  readPage(&tmpPage[2]);*/

/*  if (m_version == EEPROM_VERSION) {
	  readPage(&customPage[0]);
	  readPage(&customPage[1]);
	  readPage(&customPage[2]);
  }else{
		DEBUGLOG("EEPROM version not correct ==> use default values");
  }*/
  customPage[0].nbElement = 6;
  customPage[1].nbElement = 6;
  customPage[2].nbElement = 6;


//Custom Page

  //Predefined Page
  predefinedPage[0].set(HOUR_PAGE_ID,HOUR_PAGE_NAME,2);
  predefinedPage[0].element[0].set(Element::DATE,20,8,"",Element::SMALL,255,255,255,true);
  predefinedPage[0].element[1].set(Element::HOUR,0,24,"",Element::BIG,0,0,255,true);

  predefinedPage[1].set(OPENBAR_PAGE_ID,OPENBAR_PAGE_NAME,2);
  predefinedPage[1].element[0].set(Element::TEXT,2,12,"Open",Element::BIG,0,0,255,true);
  predefinedPage[1].element[1].set(Element::TEXT,30,27,"bar",Element::BIG,255,255,255,true);

  predefinedPage[2].set(NOEL_PAGE_ID,NOEL_PAGE_NAME,2);
  predefinedPage[2].element[0].set(Element::TEXT,0,11,"Joyeux",Element::BIG,255,0,0,true);
  predefinedPage[2].element[1].set(Element::TEXT,25,25,"Noel",Element::BIG,255,255,0,true);

  predefinedPage[3].set(NEWYEAR_PAGE_ID,NEWYEAR_PAGE_NAME,3);
  predefinedPage[3].element[0].set(Element::TEXT,24,11,"Annee",Element::MEDIUM,0,255,0,true);
  predefinedPage[3].element[1].set(Element::TEXT,10,11,"Bonne",Element::BIG,0,0,255,true);
  predefinedPage[3].element[2].set(Element::TEXT,21,28,"2020",Element::BIG,255,0,0,true);

  predefinedPage[4].set(METEO_PAGE_ID,METEO_PAGE_NAME,2);
  predefinedPage[4].element[0].set(Element::TEXT,0,11,"Meteo",Element::MEDIUM,255,0,0,true);
  predefinedPage[4].element[1].set(Element::TEXT,25,25,"Chez pas",Element::MEDIUM,255,255,0,true);

  predefinedPage[5].set(TEST_PAGE_ID,TEST_PAGE_NAME,1);
  predefinedPage[5].element[0].set(Element::TEXT,0,11,"Test",Element::MEDIUM,255,0,0,true);

  predefinedPage[6].set(CFG_PAGE_ID,CFG_PAGE_NAME,6);
  predefinedPage[6].element[0].set(Element::HOUR,0,6,"",Element::SMALL,255,0,0,true);
  predefinedPage[6].element[1].set(Element::TEMP,27,6,"",Element::SMALL,00,0,255,true);
  predefinedPage[6].element[2].set(Element::BITMAP,14,14,"tst.bmp",Element::SMALL,00,255,255,true);
  predefinedPage[6].element[3].set(Element::TEMP_TREND,40,2,"M{s}",Element::SMALL,255,255,255,true);
  predefinedPage[6].element[4].set(Element::DATE,1,12,"",Element::SMALL,00,255,0,true);
  predefinedPage[6].element[5].set(Element::WATCH,7,20,"5",Element::SMALL,255,255,0,true);

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
    DynamicJsonDocument doc(20000);
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
  String ss = "{"+toStringCfg(JSON_TEXT)+"}";
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

