

#ifndef MatrixPages_h
#define MatrixPages_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

//

#include "baseManager.h"
#include "settingManager.h"
#include "BMPFile.h"
//#include "main.h"
//#include <Ticker.h>
#include <PxMatrix.h>

#define FROM_CENTER 1
#define FROM_LEFT   2
#define FROM_RIGHT  3

#ifndef MATRIX_TIMER_NUM
#define MATRIX_TIMER_NUM 1
#endif



#ifdef ESP32

#define P_LAT 17//22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2

//portMUX_TYPE wtimerMux = portMUX_INITIALIZER_UNLOCKED;
//static int taskCore = 0;
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
    	// DEBUGLOG("stopTimer");
    	timerStop(timer);
    	timerAlarmDisable(timer);
        m_display.flushDisplay();
	    //m_display.clearDisplay();
    	delay(10);
    }
    void startTimer() {
    	// DEBUGLOG("startTimer");
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
    //boolean isTransition() {return startTransition!=0;};

    void displayPage();


    void displayScreen(Page *page);


    String buildTxt(String txt, String var);
    String buildGeneric(String txt);

    void displayWatch(Element *pElt);
    void displayTrend(Element *pElt, float trend);
    void displayBitmap(Element *pElt, String filename);
    void displayTestPage();
    void displayCfgPage();

    void print(Element *pElt, String txt);

    String getClassName(){return "MatrixPages";}

    String toString(boolean bJson){return "MatrixPages";}

    TransitionPages *tranPages;

  private:

    hw_timer_t * timer;
    uint16_t _numPage = 65000;

    gfxFont m_gfxFont;    
    Page pp;
    Page *m_pp;

    Window win;
    BmpCache m_bmpCache;
};


#endif
