
#ifndef PresenceHelper_h
#define PresenceHelper_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"
//#include <functional>

#else

#include "WProgram.h"

#endif

#include "DelayHelper.h"
#include <FunctionalInterrupt.h>

#define INPUT_SENSOR_PIN 16 //to be defined
#define PRESENCE_INERTIE_DURATION 100

/*void presenceManagement2() {
	DEBUGLOGF("Presence trigged[%d]\n", true);


}*/

class PresenceHelper : public DelayHelper
{
  public:

	void  isrInterruption() {
			handle();
		}

	PresenceHelper() : DelayHelper (){
        pinMode(INPUT_SENSOR_PIN,INPUT_PULLUP);
        //NTP.onNTPSyncEvent(std::bind(&HourManager::NTPsyncEvent, this, std::placeholders::_1));
        //attachInterrupt(INPUT_SENSOR_PIN, std::bind(&PresenceHelper::isrInterruption, this, std::placeholders::_2), CHANGE);
        attachInterrupt(INPUT_SENSOR_PIN, std::bind(&PresenceHelper::isrInterruption, this), CHANGE);
    } ;



    void handle() {

    	boolean activated  = digitalRead(INPUT_SENSOR_PIN);
    	if (activated) {
    		if (m_callBack && m_previousStateActivated == false) {
    			m_callBack(true);
    			//DEBUGLOGF("Presence detected [%d]\n",millis());
    		}
    		m_previousStateActivated = true;
    		startDelay(PRESENCE_INERTIE_DURATION);
    		//DEBUGLOGF("Presence detected [%d]\n",millis());
    	}else if (m_callBack && m_previousStateActivated==true) {
			m_callBack(false);
			m_previousStateActivated = false;
			//DEBUGLOGF("Presence NOT detected [%d]\n",millis());
		}
    }

    boolean isPresence() {
    	return !isDone();
    }

    void forceStatus(boolean pres) {
    	if (pres) startDelay(PRESENCE_INERTIE_DURATION);
    	else m_endMS = 0;
    }
    void setCallback(std::function<void(boolean)> _callBack) {
    	m_callBack = _callBack;
    }

  private:
    std::function<void(boolean)> m_callBack = 0;
    boolean m_previousStateActivated = false;


};

#endif
