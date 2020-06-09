
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

#ifdef OTA_FOR_ATOM
void OTAOnStart()
{
	digitalWrite(PIN_LED, LOW);
	mpPages->stopTimer();
	DEBUGLOG("My OTA");
	wfManager->OTAOnStart();
	digitalWrite(PIN_LED, HIGH);
}
#endif

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
	//mpPages->stopTimer();
	wfManager->getServer()->send(200, "text/json", getJson());
	//mpPages->startTimer();
	digitalWrite(PIN_LED, HIGH);
}

void saveConfiguration()
{
	//mpPages->stopTimer();
	smManager->writeData();
	//mpPages->startTimer();
	dataPage();
}

bool manageProg()
{
	String str;
	uint8_t prgId = (uint8_t)atoi(wfManager->getServer()->arg("obj").c_str());
	ProgElt *prgElt = smManager->getProg()->getProgElt(prgId);
	if (prgElt == NULL)
	{
		DEBUGLOGF("Not prog associated to [%d]\n", prgId);
		return false;
	}
	else
	{
		DEBUGLOGF("Prog associated to [%d]\n", prgId);
	}
	if ((str = wfManager->getServer()->arg("prgDuration")) != NULL)
	{
		prgElt->pdDuration = atoi(str.c_str());;
	} 
	else if ((str = wfManager->getServer()->arg("prgHour")) != NULL)
	{
		prgElt->setHourMinute(str);
	}
	else if ((str = wfManager->getServer()->arg("prgPage")) != NULL)
	{
		prgElt->pgId = atoi(str.c_str());
		if (prgElt->pgId>-1) {
			Page *pp = smManager->getPage(prgElt->pgId);
			if (pp != NULL) {
				prgElt->pgName = pp->name;
			} else {
				prgElt->pgName = "NotFound";
			}
		}else {
			prgElt->pgName = "";
		}

	}
	smManager->getProg()->sortPages();
	//wfManager->getServer()->send(200, "text/html", "ok");
	return true;
}

bool managePage()
{
	uint16_t idPage = 0;
	uint8_t nbElt = 0;
	Page *pp = NULL;
	String str;
	bool changed = false;
	if ((str = wfManager->getServer()->arg("page")) != NULL)
	{
		idPage = (uint16_t)atoi(str.c_str());
		pp = smManager->getPage(idPage);
		if (pp == NULL)
		{
			DEBUGLOGF("Not page associated to [%d]\n", idPage);
		} else  {
			DEBUGLOGF("page associated to [%d][%s]\n", idPage,pp->name.c_str());
		}
	};
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
	else if ((str = wfManager->getServer()->arg("trans")) != NULL)
	{
		pp->transition = (TransitionPages::TRANSTION_MODE) atoi(str.c_str());
		mpPages->tranPages->startTransition(pp->transition);
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
	return changed;
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
/*	uint16_t idPage = 0;
	uint8_t nbElt = 0;
	Page *pp = NULL;*/

	String str;
	if ((str = wfManager->getServer()->arg("freq")) != NULL)
	{
		smManager->displayedFrequence = (uint8_t)atoi(str.c_str());
	}
	else if ((str = wfManager->getServer()->arg("def")) != NULL)
	{
		smManager->displayedMode = str == "true";
	}
	else if (wfManager->getServer()->hasArg("prgHour") || wfManager->getServer()->hasArg("prgPage"))
	{
		changed = manageProg();
	}
	else if (wfManager->getServer()->hasArg("page"))
	{
		changed = managePage();
	}
	/*if (changed)
	{
		mpPages->displayPage();
	}*/
	phPresence->forceStatus(true);
	wfManager->getServer()->send(200, "text/html", "ok");
	//dataPage();
}

void dataPage()
{
	digitalWrite(PIN_LED, LOW);
	mpPages->stopTimer();

	DEBUGLOG("dataPage");
	delay(1000);
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

void progPage()
{
	digitalWrite(PIN_LED, LOW);
	mpPages->stopTimer();

	DEBUGLOG("Prog Page");
	wfManager->loadFromSpiffs("/prog.json");
	phPresence->forceStatus(true);
	digitalWrite(PIN_LED, HIGH);
}
