
#include "WifiManagerV2.h"

//#include "httpUpdater.h"

//#include <WiFiClient.h>
//#include "context.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


void WifiManager::restartESP() {
  ESP.restart();
}

void WifiManager::displayCredentialCollection() {
	//digitalWrite ( pinLed, LOW );

  String message =  "<html>\
    <head>\
      <title>Credentials page</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>";
  message += "<p>";
  message +="<ul>";
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      message += "<li>";
      message +=i + 1;
      message += ": ";
      message += WiFi.SSID(i);
      message += " (";
      message += WiFi.RSSI(i);
      message += ")";
#ifdef ESP8266
      message += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
#endif
#ifdef ESP32
      message += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*";
#endif

      message += "</li>";
    }
  message += "</ul>";
  message += "<form method='get' action='set'>";
  message += "<label>SSID:</label><input name='ssid' test length=32 value=\""+String(_basesmManager->m_ssid) +"\"><br>";
  message += "<label>Pass:</label><input name='pass' length=64 value=\""+String(HIDDEN_KEY) +"\"><br>";
  message += "<label>Channel number:</label><input name='privateKey' length=64 value=\""+String(HIDDEN_KEY) +"\"><br>";
  message += "<label>PublicKey:</label><input name='publicKey' length=64 value=\""+String(_basesmManager->m_publicKey) +"\"><br>";
  message += "<input type='submit'></form>";
  message += "</body></html>";

  _server->send ( 200, "text/html", message );

  //digitalWrite ( pinLed, HIGH );

}

#ifdef ESP32
 void IRAM_ATTR WifiManager::setCredential(){
#endif
#ifdef ESP8266
 void  WifiManager::setCredential(){
#endif
#ifdef MCPOC_TEST
  for (uint8_t i =0; i<getServer()->args();i++) {
	  DEBUGLOGF("[%s,%s]",getServer()->argName(i).c_str(),getServer()->arg(i).c_str());
  }
  DEBUGLOG("");
#endif

  String str = _server->arg("ssid");
  //DEBUGLOGF("[1,%s]\n",str.c_str());
  if (str.length()>0)
    strcpy(_basesmManager->m_ssid, str.c_str());
  str = _server->arg("pass");
  //DEBUGLOGF("[2,%s]\n",str.c_str());
  if (str.length()>0 && str != HIDDEN_KEY)
    strcpy(_basesmManager->m_password,str.c_str());
  //DEBUGLOGF("[3,%s]\n",str.c_str());
  str = _server->arg("privateKey");
  //DEBUGLOGF("[4,%s]\n",str.c_str());
  if (str.length()>0 && str != HIDDEN_KEY)
      strcpy(_basesmManager->m_privateKey,str.c_str());
  str = _server->arg("publicKey");
  //DEBUGLOGF("[5,%s]\n",str.c_str());
  if (str.length()>0)
      strcpy(_basesmManager->m_publicKey,str.c_str());
  _server->send ( 200, "text/html", "data recorded.restart board");
  _basesmManager->writeData();
  //restartESP();
}

void WifiManager::clearMemory(){
  _basesmManager->clearData();
  _server->send ( 200, "text/html", "data cleared");
}

void WifiManager::wifiReset(){
  _server->send ( 200, "text/html", "reseting...");
  WiFi.disconnect();
  restartESP();
}


WifiManager::WifiManager(unsigned char pinLed,  BaseSettingManager *smManager) : BaseManager(pinLed){
#ifdef ESP8266
	_server = new ESP8266WebServer(80);
#endif
#ifdef ESP32
	_server = new WebServer(80);
#endif
	_hrManager = new HourManager(2390,pinLed);
  _basesmManager = smManager;
}

wl_status_t WifiManager::begin( IPAddress ip, const char *MODULE_NAME, const char *MODULE_MDNS, const char *MODULE_MDNS_AP){
  wl_status_t connection = connectSSID(_basesmManager->m_ssid,_basesmManager->m_password,ip, MODULE_MDNS );
  if (connection==WL_CONNECTED) {
    Serial.println("connected");
    setStatus(WiFi.status(), "ssid");
    _hrManager->begin("pool.ntp.org", 1, true);
    #ifdef MCPOC_TEST
    WiFi.softAP(MODULE_MDNS,MODULE_AP_WPA);
    #else
    WiFi.softAP(MODULE_MDNS,MODULE_AP_WPA,1,true);
    #endif
  } else {
    Serial.println("Not connected");
    connectAP(MODULE_MDNS_AP);
    setStatus(WiFi.status(), "ap");
    WiFi.softAP(MODULE_MDNS_AP,MODULE_AP_WPA);
    _server->on ( "/", std::bind(&WifiManager::displayCredentialCollection, this) );
    _server->onNotFound ( std::bind(&WifiManager::displayCredentialCollection, this) );
  }


  MDNS.begin (MODULE_MDNS);
  MDNS.addService("http", "tcp", 80);

#ifdef MCPOC_TELNET
  MDNS.addService("telnet", "tcp", 23); // Telnet server RemoteDebug
  #endif
#ifdef ESP8266
  WiFi.hostname(MODULE_NAME);
#endif
#ifdef ESP32
  WiFi.setHostname(MODULE_NAME);
#endif
  _server->on ( "/clear", std::bind(&WifiManager::clearMemory, this) );
  _server->on ( "/restart", std::bind(&WifiManager::restartESP, this) );
  _server->on ( "/set", std::bind(&WifiManager::setCredential, this) );
  _server->on ( "/credential", std::bind(&WifiManager::displayCredentialCollection, this) );
  _server->on ( "/reset", std::bind(&WifiManager::wifiReset, this) );
#ifdef ESP8266
  _httpUpdater.setup(_server, ((const char *)"/firmware"), MODULE_UPDATE_LOGIN, MODULE_UPDATE_PASS);
#else
  initHttpUpdater();
#endif
  #ifdef OTA_FOR_ATOM
  setOTAForAtom();
  #endif

  _server->begin();
  return connection;
  }


void WifiManager::handleClient() {
  _server->handleClient();
  #ifdef MCPOC_TELNET
  Debug.handle();
  #endif
  #ifdef OTA_FOR_ATOM
  ArduinoOTA.handle();
  #endif

}



String WifiManager::toString(boolean bJson = STD_TEXT){
  return "Mode["+String(WiFi.getMode())+\
  "] SSID["+WiFi.SSID()+\
  "] LocalIP["+WiFi.localIP().toString()+\
  "] softIP[" + WiFi.softAPIP().toString() + \
  "] status[" + String(WiFi.status(),DEC) + "]" ;
}

wl_status_t WifiManager::connecting(char *ssid, char *pass) {
  if (strlen(ssid)==0) {
      if (!WiFi.SSID()) return WL_NO_SSID_AVAIL;
      Serial.println ( "Quick" );
#ifdef ESP8266
      ETS_UART_INTR_DISABLE();
      wifi_station_disconnect();
      ETS_UART_INTR_ENABLE();
#endif
#ifdef ESP32
      esp_wifi_disconnect();
#endif
      WiFi.begin();
  } else {
    Serial.println ( "Long" );
    WiFi.begin ( ssid, pass );
  }
  unsigned char inbTest = 0;
  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED && inbTest < 10) {
      delay ( 500 );
      Serial.print ( "." );
      Serial.print (WiFi.status());
      inbTest++;
  }
  return WiFi.status();
}

wl_status_t WifiManager::connectSSID(char *ssid, char *pass, IPAddress ip, const char *MDNS_LABEL){
  if (!ssid[0] || !pass[0])
    return WL_NO_SSID_AVAIL;

  IPAddress gateway(192,168,0,254);          // IP address of the router
  IPAddress subnet(255,255,255,0);
  IPAddress dns(8, 8, 4, 4);

  WiFi.config(ip, gateway, subnet, dns);
  //WiFi.config(ip, gateway, subnet);
  Serial.println ( "" );
  switchOn();
  if (connecting ((char*)"",(char*)"") != WL_CONNECTED)
    connecting ( ssid, pass );
  switchOff();
  return WiFi.status();
}

unsigned char WifiManager::connectAP(const char *MDNS_LABEL){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  return WiFi.status();
}



bool WifiManager::loadFromSpiffs(String path)
{
  DEBUGLOGF("loadFromSpiffs[%s]\n", path.c_str());

 /* Serial.println("spiffs directory");
String str = "";
Dir dir = SPIFFS.openDir("/");
while (dir.next()) {
    str += dir.fileName();
    str += " / ";
    str += dir.fileSize();
    str += "\r\n";
}
Serial.println(str);*/

  if (SPIFFS.exists(path.c_str()))
  {
    File dataFile = SPIFFS.open(path.c_str(), "r");
    if (_server->streamFile(dataFile, "text/html") != dataFile.size())
    {
      DEBUGLOGF("loadFromSpiffs pb[%s][%d]\n", dataFile.name(), dataFile.size());
    }
    DEBUGLOGF("loadFromSpiffs[%s][%d]\n", dataFile.name(), dataFile.size());
    dataFile.close();
  }
  else
  {
    DEBUGLOGF("loadFromSpiffs[%s] does not exist\n", path.c_str());
  }

  return true;
}

#ifdef ESP32
void WifiManager::httpUpdaterPage() {
    //_server->on("/firmware", HTTP_GET, []() {
    _server->sendHeader("Connection", "close");
    loadFromSpiffs("/firmware.html");
   //wfManager->getServer()->sendHeader("Location", "/robot.html",true);   //Redirige vers page index.html sur SPIFFS
    _server->send(200, "text/html", "");
}


void WifiManager::initHttpUpdater() {
  //WebServer *server = _server;

  _server->on ( "/firmware", std::bind(&WifiManager::httpUpdaterPage, this) );

  /*handling uploading firmware file */
  _server->on("/update", HTTP_POST, [&]() {
    _server->sendHeader("Connection", "close");
    _server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, [&]() {
    HTTPUpload& upload = _server->upload();
    if (upload.status == UPLOAD_FILE_START) {
      //Serial.printf("Update: %s\n", upload.filename.c_str());
      int command = U_FLASH;
      if ( upload.filename == "spiffs.bin") {
         command = U_SPIFFS;      
      }
      DEBUGLOGF("Update [%s][%d]\n", upload.filename.c_str(),command);
      if (!Update.begin(UPDATE_SIZE_UNKNOWN,command, LED_BUILTIN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        DEBUGLOGF("Update Success: %u\nRebooting...\n", upload.totalSize);
        //Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
 
}
#endif

#ifdef OTA_FOR_ATOM
void WifiManager::setOTAForAtom() {
  Serial.println(F("ArduinoOTA conf"));
  ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println(F("Auth Failed"));
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println(F("Begin Failed"));
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println(F("Connect Failed"));
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println(F("Receive Failed"));
      } else if (error == OTA_END_ERROR) {
        Serial.println(F("End Failed"));
      }
    });
    ArduinoOTA.begin();
    Serial.println(F("ArduinoOTA Ready"));
}
#endif
