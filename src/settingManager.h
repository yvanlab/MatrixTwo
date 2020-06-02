
#ifndef SettingManager_h
#define SettingManager_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include "ArduinoJson.h"
#include "utils.h"
#include "prog.h"

#include <EEPROM.h>
#include <baseManager.h>
#include <BaseSettingManager.h>
//#include "matrixPages.h"

class Element
{
public:
	enum OBJECT_TYPE
	{
		TEXT = 0,
		HOUR = 1,
		DATE = 2,
		WATCH = 3,
		TEMP = 4,
		TEMP_MIN = 5,
		TEMP_MAX = 6,
		TEMP_TREND = 7,
		BITMAP = 8,
		METEO_PRESSION = 9,
		METEO_ICON = 10,
		METEO_TEXT = 11,
		GENERIC_TEXT = 12,
		TEMP_EXT = 13,
		TEMP_EXT_MIN = 14,
		TEMP_EXT_MAX = 15,
		CURRENT = 16,
		DICTONS = 17
	};
	enum FONT_TYPE
	{
		SMALL = 0,
		MEDIUM = 1,
		BIG = 2
	};

	Element(){};
	void set(OBJECT_TYPE _type, uint8_t _x, uint8_t _y, String _txt, FONT_TYPE _font, uint8_t _red, uint8_t _green, uint8_t _blue, boolean _active)
	{
		type = _type;
		x = _x;
		y = _y;
		txt = _txt;
		font = _font;
		red = _red;
		green = _green;
		blue = _blue;
		active = _active;
	};
	OBJECT_TYPE type = TEXT;
	FONT_TYPE font = MEDIUM;
	boolean active = false;
	uint8_t id;
	int16_t x = 10;
	int16_t y = 10;
	int16_t xDec = 0;
	uint8_t red = 255;
	uint8_t green = 0;
	uint8_t blue = 0;
	String txt = {"defaut"};
	boolean isChanged = true;

	String toString(boolean bJson)
	{
		String ss;
		ss += "{\"id\":\"" + String(id) + "\",";
		ss += "\"font\":\"" + String(font) + "\",";
		ss += "\"type\":\"" + String(type) + "\",";
		ss += "\"active\":\"" + String(active) + "\",";
		char dataString[10];
		sprintf(dataString, "#%02X%02X%02X", red, green, blue);
		ss += "\"color\":\"" + String(dataString) + "\",";
		ss += "\"txt\":\"" + txt + "\",";
		ss += "\"x\":\"" + String(x) + "\",";
		ss += "\"y\":\"" + String(y) + "\",";
		ss += "\"red\":\"" + String(red) + "\",";
		ss += "\"green\":\"" + String(green) + "\",";
		ss += "\"blue\":\"" + String(blue) + "\"";
		ss = ss + "}";
		return ss;
	}

	void fromJson(JsonObject doc)
	{
		id = doc[F("id")];									// "0"
		font = (FONT_TYPE)(doc[F("font")].as<uint8_t>());	// "0"
		type = (OBJECT_TYPE)(doc[F("type")].as<uint8_t>()); // "1"
		active = doc[F("active")].as<uint8_t>() == 1;		// "1"
		//color =doc["color"]; // "#FF0000"
		txt = doc[F("txt")].as<String>(); // ""
		x = doc[F("x")];				  // "0"
		y = doc[F("y")];				  // "6"
		red = doc[F("red")];			  // "255"
		green = doc[F("green")];		  // "0"
		blue = doc[F("blue")];			  // "0"
		//DEBUGLOGF("Load Elt [%d]\n", id);
	}
};

#define MAX_NBR_ELEMENT 8

class Page
{
public:
	uint8_t nbElement = MAX_NBR_ELEMENT;
	boolean active = false;
	uint16_t id;
	/*String hourMinute;
	int16_t hourMinuteConverted;*/
	String name;
	TransitionPages::TRANSTION_MODE transition = TransitionPages::TRANSITION_LEFT;
	Element element[8];
	Page(){
	};

	void set(uint16_t _id, String _name, uint8_t _nbElts)
	{
		nbElement = _nbElts;
		name = _name;
		id = _id;
		for (uint8_t i = 0; i < nbElement; i++)
		{
			element[i].id = i;
		}
	};

	String toString(boolean bJson)
	{
		String ss;
		ss += "{\"id\":\"" + String(id) + "\",";
		ss += "\"name\":\"" + name + "\",";
		ss += "\"nbElts\":\"" + String(nbElement) + "\",";
		ss += "\"active\":\"" + String(active) + "\",";
		//ss += "\"hour\":\"" + hourMinute + "\",";
		ss += "\"transition\":\"" + String(transition) + "\",";

		ss += "\"obj\":[";
		for (uint8_t iElt = 0; iElt < MAX_NBR_ELEMENT; iElt++)
		{
			ss += element[iElt].toString(JSON_TEXT);
			if (iElt != MAX_NBR_ELEMENT - 1)
				ss = ss + ",";
		}
		ss = ss + "]";
		ss = ss + "}";
		return ss;
	}

	void fromJson(JsonObject doc)
	{
		name = doc[F("name")].as<String>();
		id = doc[F("id")];
		//uint8_t nbElt 	= doc[F("nbElts")];
		active = doc[F("active")].as<uint8_t>() == 1;
		//setHourMinute(doc[F("hour")].as<String>());
		transition = (TransitionPages::TRANSTION_MODE)doc[F("transition")].as<uint8_t>();
		//DEBUGLOGF("Load Page [%s]\n", name.c_str());
		JsonArray lstObj = doc["obj"];
		for (uint8_t iElt = 0; iElt < lstObj.size(); iElt++)
		{
			element[iElt].fromJson(lstObj[iElt]);
		}
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

	String getClassName() { return "SettingManager"; }

	//void sortPages();

	String toStringCfg(boolean bJson);
	String toString(boolean bJson);
	Page *getPage(uint16_t id);
	Prog *getProg() {
		return &customPrg;
	}

	static const uint8_t nbCustomPages = 11;
	Page customPage[nbCustomPages];
	uint8_t displayedPage = 0;
	uint8_t displayedMode = 0;
	uint8_t displayedFrequence = 0;

	Prog  customPrg;

	char *lstBMP[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
};

#endif
