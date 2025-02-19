/* ************************ Pin Configuration ************************
Matrix pins
 P_A 19
 P_B 23
 P_C 18
 P_D 5
 P_E 15
 P_OE 15

Presence pins
	PIN 4


Temperature pins
	SDA = 21;
	CL = 22;


Modificqtion de PxMATRIX.h (614)
inline void PxMATRIX::fillMatrixBuffer(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b,bool selected_buffer)
{
  if ((x < 0) || (x >= _width) || (y < 1) || (y >= _height))
                                       ^
									   !
*********************** REMOVE GPIO02 to upload programs  *************************************
*/


#include "main.h"

TaskHandle_t Task1;
TaskHandle_t Task2;



SettingManager *smManager; //(PIN_LED);
WifiManager *wfManager;	   //(PIN_LED, &smManager);
MatrixPages *mpPages;	   //(PIN_LED);
portMUX_TYPE wtimerMux = portMUX_INITIALIZER_UNLOCKED;
PresenceHelper *phPresence;

BMPManagerV2 *bmpMesure;		  //(PIN_LED);
thingSpeakManager *thinkSpeakMgr; //(pinLed);
Periferic *phPeriferic;
Dictons *pDictons;

#define LOG_LABEL "log"
#define TEMP_PIECE_LABEL 1 //"vmcHUM"
#define PRESSION_LABEL 2   //"vmcTEMP"
#define PRESENCE_LABEL 3   //"vmcVTS"
#define TEMP_PROC_LABEL 4  //"extTEMP"

#ifdef MCPOC_TELNET
RemoteDebug Debug;
#endif

#ifdef MCPOC_TELNET // Not in PRODUCTION
void processCmdRemoteDebug()
{
	String lastCmd = Debug.getLastCommand();
	/*  if (lastCmd == "o") {
	 dManager.activateModule(true);
	 } else if (lastCmd == "r") {
	 fpManager.recordFingerPrint(0,"yvan");
	 } else if (lastCmd == "a") {
	 fpManager.activateModule(true);
	 } else if (lastCmd == "restart") {
	 ESP.restart();
	 } else if (lastCmd == "reset") {
	 WiFi.disconnect();
	 }*/
}
#endif

void startWiFiserver()
{
	wfManager->getServer()->on("/firmware", firmware);
	wfManager->getServer()->on("/cleabr", clearEEPROM);

	if (wfManager->begin(IPAddress(MODULE_IP), MODULE_NAME, MODULE_MDNS,
						 MODULE_MDNS_AP) == WL_CONNECTED)
	{
/*		wfManager->getServer()->on("/", dataPage);
		wfManager->getServer()->onNotFound(dataPage);*/
	}
	wfManager->getServer()->on("/status", dataJson);
	wfManager->getServer()->on("/setting", dataPage);
	wfManager->getServer()->on("/setData", setData);
	wfManager->getServer()->on("/save", saveConfiguration);
	wfManager->getServer()->on("/config", configPage);
	wfManager->getServer()->on("/prog", progPage);

#ifdef OTA_FOR_ATOM
	ArduinoOTA.onStart(OTAOnStart);
#endif

	Serial.println(wfManager->toString(STD_TEXT));
	Serial.println("startWiFiserver :: end");
}

void screemManagement(void *pvParameters)
{
	DEBUGLOG("Task1 running on second core ");

	while (true)
	{
		//     DEBUGLOG("While");
		//delay (1);
		yield();
		if (mtTimer.isCustomPeriod(PROC_TWO))
		{	
			if (phPresence->isPresence())
			{
				mpPages->displayPage();
			}
		}
		if (mtTimer.is1MNPeriod(PROC_TWO) && smManager->displayedMode)
		{
			if (mpPages->nextPage()) {
				if (smManager->getProg()->getCurrentPrgElt() != NULL) {
					phPresence->forceStatus(true/*,smManager->getProg()->getCurrentPrgElt()->pgDuration*60000*/);
				}
			};
		}
		
		mtTimer.clearPeriod(PROC_TWO);
	};
}

void presenceManagement(boolean bPresent)
{
	DEBUGLOGF("Presence trigged[%d]\n", bPresent);
	if (bPresent)
	{
		mpPages->startTimer();
	}
	else
	{
		mpPages->stopTimer();
	}
}

void setup(void)
{
	//gdbstub_init();
	//node.setcpufreq(node.CPU160MHZ)
	Serial.begin(115200); //delay(500);
	Serial.println("start debuging");
	DEBUGLOGF("Frq : %d \n", ESP.getCpuFreqMHz());
	DEBUGLOGF("Temp : %f \n", temperatureRead());
	disableCore0WDT();
	disableCore1WDT();

	smManager = new SettingManager(PIN_LED);
	wfManager = new WifiManager(PIN_LED, smManager);
	phPresence = new PresenceHelper();
	phPeriferic = new Periferic(PIN_LED);
	pDictons = new Dictons();

	if (!SPIFFS.begin(true))
	{
		Serial.println("SPIFFS Mount Failed");
		return;
	}
	else
		Serial.println("SPIFFS Mount OK");

	smManager->readData();
	DEBUGLOG(smManager->toString(STD_TEXT));
	startWiFiserver();

pDictons->findNewDicton(day());

	bmpMesure = new BMPManagerV2(PIN_LED);
	bmpMesure->mesure();
	bmpMesure->set();
	DEBUGLOG("BMPManagerV2");
	DEBUGLOG(bmpMesure->toString(STD_TEXT));


//delay(2000);
	DEBUGLOG("MatrixPages");
	mpPages = new MatrixPages(PIN_LED);
	DEBUGLOG("MatrixPages :: Begin");
	mpPages->begin();
	DEBUGLOG("MatrixPages :: SetPage");
	mpPages->setPage(smManager->displayedPage);
	
	//phPeriferic->retrievePeriphericInfo();
DEBUGLOG("mtTimer :: Begin");
	mtTimer.begin(timerFrequence);
	mtTimer.setCustomMS(25);
DEBUGLOG("thinkSpeakMgr");
	thinkSpeakMgr = new thingSpeakManager(PIN_LED);
DEBUGLOG("pDictons");
	pDictons->findNewDicton(day());
	
	phPresence->setCallback(presenceManagement);

DEBUGLOG("xTaskCreatePinnedToCore");
	xTaskCreatePinnedToCore(
		screemManagement, /* Task function. */
		"Task1",		  /* name of task. */
		10000,			  /* Stack size of task */
		NULL,			  /* parameter of the task */
		1,				  /* priority of the task */
		&Task1,			  /* Task handle to keep track of created task */
		0);				  /* pin task to core 0 */

//delay(1000);
phPresence->forceStatus(true);

#ifdef MCPOC_TELNET
	Debug.begin(MODULE_NAME);
	String helpCmd = "l1\n\rl2\n\rl3\n\rrestart\n\rreset\n\r";
	Debug.setHelpProjectsCmds(helpCmd);
	Debug.setCallBackProjectCmds(&processCmdRemoteDebug);
#endif

#ifdef MCPOC_MOCK
	randomSeed(analogRead(0));
#endif

#ifdef MCPOC_TEST
	//Serial.println("Testing device connections...");
#endif
}
uint8_t iDay = 0;

void /*RAMFUNC*/ loop(void)
{
	wfManager->handleClient();
	phPresence->handle();

	/*	if (mtTimer.isCustomPeriod())
	{
		if (phPresence->isPresence())
		{
			mpPages->displayPage();
		}
	}
	if (mtTimer.is1MNPeriod() && smManager->displayedMode)
	{
		mpPages->nextPage();
	}

	*/
	if (mtTimer.is5MNPeriod(PROC_ONE))
	{
		if (!phPresence->isPresence())
		{
			bmpMesure->mesure();
			bmpMesure->set();
			phPeriferic->retrievePeriphericInfo();
		}
	}

	/*if (mtTimer.is1MNPeriod(PROC_ONE) && smManager->displayedMode)
	{
		mpPages->nextPage();
	}*/

#ifdef MCPOC_TEST
	if (Serial.available())
	{
		char c = Serial.read();
		Serial.print(c);
		if (c == 'p')
		{
			phPeriferic->retrievePeriphericInfo();
		}
		else if (c == 'd')
		{
			iDay++;
			mpPages->stopTimer();
			pDictons->findNewDicton(iDay);
			mpPages->startTimer();
		}
		else if (c == 's')
		{
			smManager->getProg()->sortPages();
		}
		else if (c == 'n')
		{
			mpPages->nextPage();
		}
	}
#endif

	if (mtTimer.isCustomPeriod(PROC_ONE))
	{
		//m_display->clearDisplay();
		//DEBUGLOG("1s");
		//bmpMesure->mesure();

		//DEBUGLOG("BMPManagerV2");DEBUGLOG(bmpMesure->toString(STD_TEXT));
	}
	if (mtTimer.is5MNPeriod(PROC_ONE))
	{
		/*if (WiFi.isConnected())
		{
			thinkSpeakMgr->addVariable(TEMP_PIECE_LABEL, String(bmpMesure->getTemperatureSensor()->getValue()));
			thinkSpeakMgr->addVariable(PRESSION_LABEL, String(bmpMesure->getPressionSensor()->getValue()));
			thinkSpeakMgr->addVariable(PRESENCE_LABEL, String(phPresence->isPresence()));
			thinkSpeakMgr->addVariable(TEMP_PROC_LABEL, String(temperatureRead()));
			thinkSpeakMgr->sendIoT(smManager->m_privateKey, smManager->m_publicKey);	
		}*/
	}

	if (wfManager->getHourManager()->isNextDay())
	{
		ESP.restart();
	}

	if (mtTimer.is5MNPeriod(PROC_ONE))
	{
		if (!WiFi.isConnected())
		{
			ESP.restart();
		}
	}



	mtTimer.clearPeriod(PROC_ONE);
}
