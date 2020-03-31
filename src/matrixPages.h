

#ifndef MatrixPages_h
#define MatrixPages_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

//

#include "BaseManager.h"
#include "SettingManager.h"
//#include "main.h"
//#include <Ticker.h>
#include <PxMatrix.h>

#define FROM_CENTER 1
#define FROM_LEFT   2
#define FROM_RIGHT  3




#ifdef ESP32

#define P_LAT 17//22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2

//portMUX_TYPE wtimerMux = portMUX_INITIALIZER_UNLOCKED;
static int taskCore = 0;
#endif

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16 //D0
 #define P_A D1 //5	//D1 ==>D9 ==>D3
 #define P_B D2 //4	//D2 ==>D10
 #define P_C 15	//D8
 #define P_OE 2	//D4
 #define P_D 12	//D6
 #define P_E 0	//D3

#endif



//static PxMATRIX *m_display;//(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);
static PxMATRIX m_display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);


class MatrixPages : public BaseManager
{
  public:
    MatrixPages(unsigned char pinLed);
    void begin();
#ifdef ESP8266
    static void display_updater();

#endif
#ifdef ESP32
    static void IRAM_ATTR display_updater();
#endif
    void stopTimer() {
    	 DEBUGLOG("stopTimer");
    	timerStop(timer);
    	timerAlarmDisable(timer);
    	delay(10);
    }
    void startTimer() {
    	 DEBUGLOG("startTimer");
    	timerStart(timer);
        timerAlarmEnable(timer);
        delay(10);
        }

    void IRAM_ATTR display(){
    	m_display.display(30);
    }

    //void display_updater();
    void nextPage();
    void setPage(uint16_t num);
    boolean isTransition() {return startTransition!=0;};

    void displayPage();
    void displayMessagePage();
    void displayCurrentPage();
    void displayVMCPage();
    void displayMeteoPage();
    void displayCfgPage();
    void displayStopPage();
    void displayHourPage();


    void displayScreen(Page *page);
    void displayOpenBar();
    void displayJoyeuxNoel();
    void displayBonneAnnee();
    void displayCustomPage();

    String getClassName(){return "MatrixPages";}

    String toString(boolean bJson);

    uint16_t page = 0;
    uint16_t m_myCOLORS[8];
    uint8_t m_colorIndex = 0;


  private:
    boolean manageTransition(byte transitionType);
    int16_t xDecalage = 128;
    uint8_t previous_detection = 0;
    int16_t startTransition=0;

    hw_timer_t * timer;

    //portMUX_TYPE timerMux;// = portMUX_INITIALIZER_UNLOCKED;
    //Periferic *periferic;
    //Ticker m_display_ticker;

    Page pp;
};


#endif
