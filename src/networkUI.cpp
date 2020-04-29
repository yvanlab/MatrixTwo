
#include "main.h"

String getJson()
{
	DEBUGLOG("getJson");
	//portENTER_CRITICAL_ISR(&wtimerMux);

	String tt("{\"module\":{");
	tt += "\"nom\":\"" + String(MODULE_NAME) + "\",";
	tt += "\"version\":\"" + String(LA_MATRIX2_VERSION) + "\",";
	tt += "\"status\":\"" + String(STATUS_PERIPHERIC_WORKING) + "\",";
	tt += "\"uptime\":\"" + NTP.getUptimeString() + "\",";
	tt += "\"datetime\":{" + wfManager->getHourManager()->toDTString(JSON_TEXT) + "},";
	tt += "\"temp\":\"" + String((temprature_sens_read() - 32) / 1.8) + "\",";
	tt += "\"build_date\":\"" + String(__DATE__ " " __TIME__) + "\"},";

	tt += "\"setting\":{" + smManager->toString(JSON_TEXT) + "},";
	tt += "\"bmp\":{" + bmpMesure->toString(JSON_TEXT) + "},";
	tt += "\"LOG\":[" + wfManager->log(JSON_TEXT) + "," + smManager->log(JSON_TEXT) + /*","+bmpMesure->log(JSON_TEXT)+*/ "]";
	tt += "}";

	//portEXIT_CRITICAL_ISR(&wtimerMux);
	return tt;
}

void firmware()
{
	digitalWrite(PIN_LED, LOW);
	mpPages->stopTimer();

	DEBUGLOG("My  firmware");
	wfManager->httpUpdaterPage();
	//wfManager->getServer()->send_P ( 200, "text/html", HTML );

	//mpPages->startTimer();
	digitalWrite(PIN_LED, HIGH);
}

void clearEEPROM()
{
	digitalWrite(PIN_LED, LOW);
	mpPages->stopTimer();

	DEBUGLOG("My  firmware");
	smManager->clearData();
	//wfManager->getServer()->send_P ( 200, "text/html", HTML );

	//mpPages->startTimer();
	digitalWrite(PIN_LED, HIGH);
	phPresence->forceStatus(true);
}

void dataJson()
{
	digitalWrite(PIN_LED, LOW);
	wfManager->getServer()->send(200, "text/json", getJson());
	digitalWrite(PIN_LED, HIGH);
}

void saveConfiguration()
{
	//mpPages->stopTimer();
	smManager->writeData();
	//mpPages->startTimer();
	dataPage();
}

void setData()
{
	bool changed = false;
	//String str;

	DEBUGLOG("SetData");
#ifdef MCPOC_TEST
	for (uint8_t i = 0; i < wfManager->getServer()->args(); i++)
	{
		DEBUGLOGF("[%s,%s]", wfManager->getServer()->argName(i).c_str(), wfManager->getServer()->arg(i).c_str());
	}
	DEBUGLOG("");
#endif
	uint16_t idPage = 0;
	uint8_t nbElt = 0;
	Page *pp = NULL;

	String str;
	if ((str = wfManager->getServer()->arg("freq")) != NULL)
	{
		smManager->displayedFrequence = (uint8_t)atoi(str.c_str());
		return;
	}
	
	if ((str = wfManager->getServer()->arg("def")) != NULL)
	{
		smManager->displayedMode = (uint8_t)atoi(str.c_str());
		return;
	}

	
	if ((str = wfManager->getServer()->arg("page")) != NULL)
	{
		idPage = (uint16_t)atoi(str.c_str());
		pp = smManager->getPage(idPage);
		if (pp == NULL) {
			DEBUGLOGF("Not page associated to [%d]\n", idPage);
		} else  {
			DEBUGLOGF("page associated to [%d]\n", idPage);
		}
	}

	;

	if ((str = wfManager->getServer()->arg("obj")) != NULL)
	{
		nbElt = (uint8_t)atoi(str.c_str());
	}

	if ((str = wfManager->getServer()->arg("activatepage")) != NULL)
	{
		pp->active = str == "true";
		changed = true;
	}
	else if ((str = wfManager->getServer()->arg("displaypage")) != NULL)
	{
		mpPages->setPage(idPage);
		smManager->displayedPage = idPage;
		changed = true;
	}
	else if ((str = wfManager->getServer()->arg("name")) != NULL)
	{
		pp->name = str;
		changed = true;
	}
	else if ((str = wfManager->getServer()->arg("hour")) != NULL)
	{
		pp->setHourMinute(str);
		changed = true;
	}
	else
	{
		if ((str = wfManager->getServer()->arg("x")) != NULL)
		{
			pp->element[nbElt].x = (uint16_t)atoi(str.c_str());
			changed = true;
		}

		
		if ((str = wfManager->getServer()->arg("y")) != NULL)
		{
			pp->element[nbElt].y = (uint16_t)atoi(str.c_str());
			changed = true;
		}

		
		if ((str = wfManager->getServer()->arg("color")) != NULL)
		{
			uint32_t col = strtoul(str.c_str(), NULL, 16);
			DEBUGLOGF("color [%s],[%s]\n", String(col, HEX).c_str(), str.c_str());
			pp->element[nbElt].red = (uint8_t)((col & 0xFF0000) >> 16);
			pp->element[nbElt].green = (uint8_t)((col & 0x00FF00) >> 8);
			pp->element[nbElt].blue = (uint8_t)(col & 0x0000FF);
			changed = true;
		}

		if (wfManager->getServer()->hasArg("txt"))
		{
			
			if ((str = wfManager->getServer()->arg("txt")) == NULL)
			{
				str = "";
			}
			if (pp->element[nbElt].type == Element::BITMAP && str != pp->element[nbElt].txt)
			{
				pp->element[nbElt].isChanged = true;
			}

			pp->element[nbElt].txt = str;
			changed = true;
		}

		
		if ((str = wfManager->getServer()->arg("font")) != NULL)
		{
			pp->element[nbElt].font = (Element::FONT_TYPE)atoi(str.c_str());
			changed = true;
		}

		
		if ((str = wfManager->getServer()->arg("type")) != NULL)
		{
			pp->element[nbElt].type = (Element::OBJECT_TYPE)atoi(str.c_str());
			changed = true;
		}
		
		if ((str = wfManager->getServer()->arg("active")) != NULL)
		{
			pp->element[nbElt].active = str == "true";
			changed = true;
		}
	}

	if (changed)
	{
		mpPages->displayPage();
	}
	phPresence->forceStatus(true);
	wfManager->getServer()->send(200, "text/html", "ok");
	//dataPage();
}

void dataPage()
{
	digitalWrite(PIN_LED, LOW);
	mpPages->stopTimer();

	DEBUGLOG("dataPage");
	wfManager->loadFromSpiffs("/index.html");
	//wfManager->getServer()->send_P ( 200, "text/html", HTML );

	phPresence->forceStatus(true);
	digitalWrite(PIN_LED, HIGH);
}

void configPage()
{
	digitalWrite(PIN_LED, LOW);
	mpPages->stopTimer();

	DEBUGLOG("config Page");
	wfManager->loadFromSpiffs("/config.json");
	phPresence->forceStatus(true);
	digitalWrite(PIN_LED, HIGH);
}
