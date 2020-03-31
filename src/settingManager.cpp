

#include "SettingManager.h"
#include "main.h"


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

unsigned char SettingManager::readData(){
  BaseSettingManager::readData();
  switchOn();
  displayedPage = readEEPROM();
  displayedMode = readEEPROM();
  displayedFrequence = readEEPROM();


  customPage[0].set(CUSTOM_0_PAGE_ID, "cust1", 3);
  customPage[1].set(CUSTOM_1_PAGE_ID, "cust2", 3);
  customPage[2].set(CUSTOM_2_PAGE_ID, "cust3", 3);
  /*Page 	tmpPage[3];
  readPage(&tmpPage[0]);
  readPage(&tmpPage[1]);
  readPage(&tmpPage[2]);*/

  if (m_version == EEPROM_VERSION) {
	  readPage(&customPage[0]);
	  readPage(&customPage[1]);
	  readPage(&customPage[2]);
  }else{
		DEBUGLOG("EEPROM version not correct ==> use default values");
  }
//Custom Page

  //Predefined Page
  predefinedPage[0].set(HOUR_PAGE_ID,HOUR_PAGE_NAME,2);
  predefinedPage[0].element[0].set(Element::DATE,21,8,"",Element::SMALL,255,255,255,true);
  predefinedPage[0].element[1].set(Element::HOUR,0,24,"",Element::BIG,0,0,255,true);

  predefinedPage[1].set(OPENBAR_PAGE_ID,OPENBAR_PAGE_NAME,2);
  predefinedPage[1].element[0].set(Element::TEXT,2,12,"Open",Element::BIG,0,0,255,true);
  predefinedPage[1].element[1].set(Element::TEXT,30,27,"bar",Element::BIG,255,255,255,true);

  switchOff();
  return m_iEEprom;
}

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
}

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


unsigned char SettingManager::writeData(){
  mpPages->stopTimer();

  BaseSettingManager::writeData();
  switchOn();

  //active
  writeEEPROM(displayedPage);
  writeEEPROM(displayedMode);
  writeEEPROM(displayedFrequence);

  for (uint8_t p=0 ;p<nbCustomPages ;p++) {
	  writePage(&customPage[p]);
  }

  DEBUGLOGF("SettingManager::writeData() [%d)]\n",m_iEEprom );
  //portENTER_CRITICAL_ISR(&wtimerMux);
  EEPROM.commit();
  //portEXIT_CRITICAL_ISR(&wtimerMux);
  DEBUGLOGF("SettingManager::writeData() Commit [%d)]\n",m_iEEprom );
  setStatus( m_iEEprom, "written");
  mpPages->startTimer();
  switchOff();
  return m_iEEprom;
}

