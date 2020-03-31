
#include "HourManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



HourManager::HourManager(unsigned int localPort, unsigned char pinLed) : BaseManager(pinLed) {
  m_localPort = localPort;

	//WiFi.onStationModeDisconnected(std::bind(&HourManager::onWifiDeConnectonEvent, this,_1));
}



boolean  HourManager::isNextDay() {
	if (!m_hourSynchronised)
			return false;

		if (m_currentDay> 0 && day() != m_currentDay) {
			m_currentDay = day();
			return true;
		}
		return false;
}

boolean HourManager::isNextHour() {
	if (m_currentHour>=0 && day() != m_currentHour) {
		m_currentHour = hour();
		return true;
	}
	return false;
}

boolean HourManager::isNextMinute() {
	if (m_currentMinute>=0 && minute() != m_currentMinute) {
		m_currentMinute = minute();
		return true;
	}
	return false;
}

String HourManager::toDTString(boolean bJson) {
  if (bJson==STD_TEXT) {
    return NTP.getTimeDateString();
  } else {
    return String ( "\"date\":\""+NTP.getDateStr()+"\",\"time\":\""+NTP.getTimeStr()+"\"");
  }
  return "";
}


String HourManager::toUTString(){
  return NTP.getUptimeString();
  //return "";
}


void HourManager::NTPsyncEvent(NTPSyncEvent_t ntpEvent) {
  if (ntpEvent) {
    DEBUGLOG("**Time Sync error: ");
    if (ntpEvent == noResponse) {
      setStatus(millis(), ntpEvent, "NTP server not reachable");
      DEBUGLOG("NTP server not reachable");
    }
    else if (ntpEvent == invalidAddress){
      setStatus(millis(), ntpEvent, "Invalid NTP server address");
      DEBUGLOG("**Invalid NTP server address");
    }
  }
  else {
		DEBUGLOG("**Got NTP time: ");
		setStatus(0, "Synchronised");
		DEBUGLOG(NTP.getTimeDateString(NTP.getLastNTPSync()));
		m_hourSynchronised = true;
		m_currentDay = day();
		m_currentHour = hour();
		m_currentMinute = minute();
	}
}


bool HourManager::begin(String ntpServerName, int timeOffset, bool daylight) {

#ifdef ESP8266
  NTP.onNTPSyncEvent(std::bind(&HourManager::NTPsyncEvent, this,_1));
#endif
#ifdef ESP32
  //NTP.onNTPSyncEvent(std::bind(&HourManager::NTPsyncEvent, this,_1));
  NTP.onNTPSyncEvent(std::bind(&HourManager::NTPsyncEvent, this, std::placeholders::_1));
#endif

  NTP.begin(ntpServerName, timeOffset, daylight);
  NTP.setInterval(60*60); //every hour

  return true;
}
