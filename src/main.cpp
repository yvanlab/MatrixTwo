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
}

void screemManagement(void *pvParameters)
{
	DEBUGLOG("Task1 running on second core ");

	while (true)
	{
		if (mtTimer.isCustomPeriod())
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
		//		delay(1);
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

	mpPages = new MatrixPages(PIN_LED);
	mpPages->begin();
	mpPages->setPage(smManager->displayedPage);
	//mpPages->displayPage();

	//pDictons->findNewDicton(day());

	bmpMesure = new BMPManagerV2(PIN_LED);
	bmpMesure->mesure();
	bmpMesure->set();
	DEBUGLOG("BMPManagerV2");
	DEBUGLOG(bmpMesure->toString(STD_TEXT));

	//phPeriferic->retrievePeriphericInfo();

	mtTimer.begin(timerFrequence);
	mtTimer.setCustomMS(25);

	thinkSpeakMgr = new thingSpeakManager(PIN_LED);

	pDictons->findNewDicton(day());

	xTaskCreatePinnedToCore(
		screemManagement, /* Task function. */
		"Task1",		  /* name of task. */
		10000,			  /* Stack size of task */
		NULL,			  /* parameter of the task */
		1,				  /* priority of the task */
		&Task1,			  /* Task handle to keep track of created task */
		0);				  /* pin task to core 0 */

	delay(2000);
	phPresence->setCallback(presenceManagement);
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
	if (mtTimer.is5MNPeriod())
	{
		if (!phPresence->isPresence())
		{
			bmpMesure->mesure();
			bmpMesure->set();
			phPeriferic->retrievePeriphericInfo();
		}
	}

	if (mtTimer.is1MNPeriod() && smManager->displayedMode)
	{
		mpPages->nextPage();
	/*	phPresence->set
		phPresence->forceStatus(true);*/
	}

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

	if (mtTimer.isCustomPeriod())
	{
		//m_display->clearDisplay();
		//DEBUGLOG("1s");
		//bmpMesure->mesure();

		//DEBUGLOG("BMPManagerV2");DEBUGLOG(bmpMesure->toString(STD_TEXT));
	}
	if (mtTimer.is5MNPeriod())
	{
		if (WiFi.isConnected())
		{
			thinkSpeakMgr->addVariable(TEMP_PIECE_LABEL, String(bmpMesure->getTemperatureSensor()->getValue()));
			thinkSpeakMgr->addVariable(PRESSION_LABEL, String(bmpMesure->getPressionSensor()->getValue()));
			thinkSpeakMgr->addVariable(PRESENCE_LABEL, String(phPresence->isPresence()));
			thinkSpeakMgr->addVariable(TEMP_PROC_LABEL, String(temperatureRead()));
			thinkSpeakMgr->sendIoT(smManager->m_privateKey, smManager->m_publicKey);

			// second thinkSpeakMgr->sendIoT( smManager->m_privateKey, smManager->m_publicKey);
		}
	}

	if (wfManager->getHourManager()->isNextDay())
	{
		ESP.restart();
	}

	if (mtTimer.is5MNPeriod())
	{
		if (!WiFi.isConnected())
		{
			ESP.restart();
		}
	}

	/*
	if (mtTimer.is25MSPeriod()) {
	    DEBUGLOG("25ms");
	    siInterface.checkPersonDetected();
	    siInterface.checkPageChangeDetected();
	    if (siInterface.isCfgDetected()) {
	      mpPages->setPage(CFG_PAGE);
	      //DEBUGLOG("cfg");
	    }else if (siInterface.isPagechangeDetected() ){
	      DEBUGLOG("next pp");
	      mpPages->nextPage();
	      DEBUGLOGF("next pp : %d\n",mpPages->page );
	    } else if (siInterface.isPersonDetected() ){
	        matrix.switchScreenStatus(SWITCH_ON_SCREEN);
	        if (wfManager->getHourManager()->isNextHour()) {
	          String text;
	          text = "il est " + String(hour()) + " heure";// et " + String(minute()) + " minute." + FPSTR (weatherString[pPeriferic.getTrendMeteo()]);
	          //sprintf(text,)
	          pPeriferic.sendToVoiceBox(text);
	        }

	    } else {
	      //DEBUGLOG("nobody detected");
	      matrix.switchScreenStatus(SWITCH_OFF_SCREEN);
	      mpPages->setPage(MESSAGE_PAGE);
	    }
	  }
	  if (matrix.isScreenActivated()) {
	    mpPages->displayPage();
	    matrix.displayScreen();

	  } else {
	    if (mtTimer.is1MNPeriod()){
	      pPeriferic.retrievePeriphericInfo();
	    }
	  }

	if (mtTimer.is1MNPeriod()){
	  boolean nowDetection = siInterface.isPersonDetected();

	  if (((!previousPresence && nowDetection) || (previousPresence && !nowDetection)) || mtTimer.is30MNPeriod()  ) {
	    sfManager.addVariable(PRESENCE_LABEL, String (nowDetection));
	    DEBUGLOG(sfManager.toString(STD_TEXT));
	    int res = sfManager.sendIoT( smManager.m_privateKey, smManager.m_publicKey);
	    DEBUGLOGF("sendIoT : %d\n",res );
	    previousPresence = nowDetection;
	  }
	}
*/

	mtTimer.clearPeriod();
}
