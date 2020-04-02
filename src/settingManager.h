

#ifndef SettingManager_h
#define SettingManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#define TEMPERATURE_PAGE_ID    0b0000000000000001 //0x1
#define TEMPERATURE_PAGE_NAME  "Temperature"

#define METEO_PAGE_ID  		0b0000000000000010 //0x2
#define METEO_PAGE_NAME   	"Meteo"

#define OPENBAR_PAGE_ID		0b0000000000000100 //0x4
#define OPENBAR_PAGE_NAME	"Open Bar"

#define NOEL_PAGE_ID   		0b0000000000001000 //0x8
#define NOEL_PAGE_NAME 		"Noel"

#define NEWYEAR_PAGE_ID		0b0000000000010000 //0x10
#define NEWYEAR_PAGE_NAME	"Newyear"

#define HOUR_PAGE_ID   		0b0000000000100000 //0x20
#define HOUR_PAGE_NAME   	"Heure"

#define CLOSEBAR_PAGE_ID   	0b0000000001000000 //0x40
#define HAPPYHOUR_PAGE_NAME "Happy hour"


#define CUSTOM_0_PAGE_ID  		0b0000000100000000 //0x40
#define CUSTOM_1_PAGE_ID  		0b0000001000000000 //0x80
#define CUSTOM_2_PAGE_ID  		0b0000010000000000 //0x100


#define TEST_PAGE_ID 	    0b0100000000000000
#define TEST_PAGE_NAME 	    "Test"

#define CFG_PAGE_ID     	    0b1000000000000000 //0x8000
#define CFG_PAGE_NAME     	    "cfg"

#define ETEINDRE_PAGE_ID 	    0b0000000000000000
#define ETEINDRE_PAGE_NAME 	    "Eteindre"




#include <EEPROM.h>
#include <baseManager.h>
#include <BaseSettingManager.h>

class Element
{
public:
	enum OBJECT_TYPE {TEXT=0, HOUR=1, DATE=2,TEMP=3, TEMP_MIN=4, TEMP_MAX=5, TEMP_TREND=6};
	enum FONT_TYPE {SMALL=0, MEDIUM=1, BIG=2};

	Element () {};
	void set (OBJECT_TYPE _type, uint8_t  _x,uint8_t _y, String _txt,  FONT_TYPE _font, uint8_t _red, uint8_t _green, uint8_t _blue, boolean _active  )
		{type =_type;  x = _x;  y=_y; txt=_txt; font = _font; red=_red; green=_green; blue=_blue; active=_active;  };
	OBJECT_TYPE  type = TEXT;
	FONT_TYPE font = MEDIUM;
	boolean active =false;
	uint8_t id;
	int16_t  x=10;
	int16_t  y=10;
	int16_t  xDec;
	uint8_t  red=255;
	uint8_t  green = 0;
	uint8_t  blue=0;
	String 	 txt={"defaut"};

	String toString(boolean bJson) {
		String ss;
		ss += "{\"id\":\""+String(id)+ "\"," ;
		ss +=  "\"font\":\""+ String(font) + "\"," ;
		ss +=  "\"type\":\""+ String(type) + "\"," ;
		ss +=  "\"active\":\""+ String(active) + "\"," ;
		char dataString[10];
		sprintf(dataString,"#%02X%02X%02X",red,green,blue);
		ss +=  "\"color\":\""+ String(dataString) +  "\"," ;
		ss +=  "\"txt\":\"."+ txt + "\"," ;
		ss +=  "\"x\":\""+ String(x) + "\"," ;
		ss +=  "\"y\":\""+ String(y) + "\"," ;
		ss += "\"red\":\""+ String(red) + "\"," ;
		ss += "\"green\":\""+ String(green) + "\"," ;
		ss += "\"blue\":\""+ String(blue)+ "\"" ;
		ss = ss + "}";
		return ss;
	}

};

class Page
{
public:
	uint8_t nbElement = 3;
	uint8_t active = 0;
	uint16_t id;
	String name;
	Element element[3];
	Page () {};

	void set(uint16_t _id, String _name, uint8_t _nbElts) {
		nbElement = _nbElts;
		name = _name;
		id = _id;
		for (uint8_t i=0; i<nbElement; i++){
			element[i].id = i;
		}
	};

	String toString(boolean bJson) {
		String ss;
		ss += "{\"id\":\""+String(id)+ "\"," ;
		ss += "\"name\":\""+name+ "\"," ;
		ss += "\"nbElts\":\""+String(nbElement)+ "\"," ;
		ss += "\"active\":\""+String(active)+ "\"," ;

		ss += "\"obj\":[";
		for (uint8_t iElt=0; iElt<nbElement;iElt++) {
			ss += element[iElt].toString(JSON_TEXT);
			if (iElt!=nbElement-1)ss = ss + ",";
		}
		ss = ss + "]";
		ss = ss + "}";
		return ss;
	}


};




class SettingManager : public BaseSettingManager
{
  public:

    SettingManager(unsigned char pinLed);
    virtual uint8_t readData();
    virtual uint8_t writeData();

    void writePage(Page *page);
    void readPage(Page *page);

    String getClassName(){return "SettingManager";}
    String toString(boolean bJson){
      String ss;
      if (bJson ==STD_TEXT) {
        ss = BaseSettingManager::toString(bJson);
      } else {
    	  ss = "\"displayedPage\":\"" + String(displayedPage) + "\",";
    	  ss += "\"displayedMode\":\"" + String(displayedMode) + "\",";
    	  ss += "\"displayedFreq\":\"" + String(displayedMode) + "\",";

    	  ss += "\"page\":[";
    	  for (uint8_t iPage = 0; iPage<nbCustomPages; iPage++ ) {
    		  ss += customPage[iPage].toString(JSON_TEXT);
    		  if (iPage!=nbCustomPages-1)ss = ss + ",";
    	  }
    	  ss = ss + "],";

    	  //predefined pqge
    	  ss += "\"predefpage\":[";
    	  for (uint8_t iPage = 0; iPage<predefnbPages; iPage++ ) {
    		  ss += predefinedPage[iPage].toString(JSON_TEXT);
    		  if (iPage!=predefnbPages-1)ss = ss + ",";
    	    }
    	    ss = ss + "]";
       }
      return ss;
    }

    Page *getPage(uint16_t id) {
    	for (uint8_t iPage = 0; iPage<predefnbPages; iPage++ ) {
    		//DEBUGLOGF("predefinedPage [%d/%d]\n",predefinedPage[iPage].id,id);
    		if (predefinedPage[iPage].id == id) {
    			return &predefinedPage[iPage];
    		}
    	}
    	for (uint8_t iPage = 0; iPage<nbCustomPages; iPage++ ) {
    		//DEBUGLOGF("customPage [%d/%d]\n",customPage[iPage].id,id);
        		if (customPage[iPage].id == id) {
        			return &customPage[iPage];
        		}
        	}
    	return NULL;
    }



    static const uint8_t nbCustomPages = 3;
    static const uint8_t predefnbPages = 6;
    Page 	customPage[nbCustomPages];
    Page 	predefinedPage[predefnbPages];

    uint8_t displayedPage = 0;
    uint8_t displayedMode = 0;
    uint8_t displayedFrequence = 0;


};

#endif
