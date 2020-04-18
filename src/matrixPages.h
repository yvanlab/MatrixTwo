

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

class Window
{
public:
    int16_t _x,_y;
    int16_t _w,_h;
    int16_t getX(int16_t x) {return  _x+x;};
    int16_t getY(int16_t y) {return  _y+y;};
    int16_t getW() {return  _w;};
    int16_t getH() {return  _h;};
    void set(int16_t x, int16_t y, int16_t w, int16_t h) {
        _x=x;_y=y;_w=w;_h=h;
    };
    void setX(int16_t x) {_x=x;};
    void setY(int16_t y) {_y=y;};
    void setW(int16_t w) {_w=w;};
    void setH(int16_t h) {_h=h;};    
};

class TransitionPages
{
    public:
    enum TRANSTION_MODE {TRANSITION_LEFT, TRANSITION_RIGHT, TRANSITION_UP,TRANSITION_DOWN,TRANSITION_FROM_CENTER,TRANSITION_ROTATION };
    TransitionPages(Window *win, int16_t maxW, int16_t maxH) {
        _win  = win;
        _maxW = maxW;
        _maxH = maxH;
    }

    void startTransition(TRANSTION_MODE mode) {
        _isTransitionMode = true;
        _mode = mode;
        if (_mode == TRANSITION_LEFT) {
            _win->set(_maxW,0,_maxW,_maxH);
        } else if (_mode == TRANSITION_RIGHT) {
            _win->set(-1*_maxW,0,_maxW,_maxH);           
        } else if (_mode == TRANSITION_ROTATION) {
            _tmpInc = 180;
        } else if (_mode == TRANSITION_UP) {
            _win->set(0,_maxH,_maxW,_maxH);           
        } else if (_mode == TRANSITION_DOWN) {
            _win->set(0,-1*_maxH,_maxW,_maxH);           
        }
    };
    
    void nextStep() {
        if (!_isTransitionMode) return;

        if (_mode == TRANSITION_LEFT) {
            _tmpInc = _win->_x-1;
            _win->setX(_tmpInc);
        } else if (_mode == TRANSITION_RIGHT) {
            _tmpInc = _win->_x+1;
            _win->setX(_tmpInc);
        }  else if (_mode == TRANSITION_UP) {
            _tmpInc = _win->_y-1;
            _win->setY(_tmpInc);
        } else if (_mode == TRANSITION_DOWN) {
            _tmpInc = _win->_y+1;
            _win->setY(_tmpInc);
        } else if (_mode == TRANSITION_ROTATION) {
            _tmpInc-=10;
            _win->_x = _maxW/4-(_maxW/4)*cos(radians(_tmpInc));
            _win->_y = (_maxH/4)*sin(radians(_tmpInc));            
/*            if (_tmpInc==0) {
                _isTransitionMode = false;
                _win->_x = 0;
                _win->_y = 0;
            } */
        }

        if (_tmpInc == 0) {
            _isTransitionMode = false;
        }
    };

    boolean isTransition() {return _isTransitionMode;};

    private:
    TRANSTION_MODE _mode;
    boolean _isTransitionMode= false;
    Window  *_win;
    int16_t _maxW;
    int16_t _maxH;
    int16_t _tmpInc;
};




class gfxFont 
{
    public:
    GFXfont *m_gfxFont = NULL;
	uint8_t m_first, m_last;

    void set(GFXfont *gfxFont = NULL) {
        m_gfxFont   = gfxFont;
        if (m_gfxFont) {
	        m_first     = pgm_read_byte(&m_gfxFont->first),
            m_last      = pgm_read_byte(&m_gfxFont->last);      
        }
    };

    int16_t getXadvance(char c) {
		if (m_gfxFont) {
            if ((c >= m_first) && (c <= m_last)) { // Char present in this font?
    		    GFXglyph *glyph = m_gfxFont->glyph + c - m_first;// pgm_read_glyph_ptr(gfxFont, c - first);
                return (int16_t)pgm_read_byte(&glyph->xAdvance);
            }
            return 0;
        } else {
            return 6;
        }

    };

};


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
    boolean isTransition() {return startTransition!=0;};

    void displayPage();


    void displayScreen(Page *page);


    String buildTxt(String txt, String var);
    String buildGeneric(String txt);

    void displayWatch(Element *pElt);
    void displayTrend(Element *pElt, float trend);
    void displayBitmap(Element *pElt);
    void displayTestPage();
    void displayCfgPage();

    void print(Element *pElt, String txt);

    String getClassName(){return "MatrixPages";}

    String toString(boolean bJson){return "MatrixPages";}


  private:
    boolean manageTransition(byte transitionType);
    int16_t xDecalage = 128;
    uint8_t previous_detection = 0;
    int16_t startTransition=0;

    hw_timer_t * timer;
    uint16_t _numPage = 0;
    BMPFile *bitmapCache[6] = {NULL,NULL,NULL,NULL,NULL,NULL};

    gfxFont m_gfxFont;    
    Page pp;

    Window win;
    TransitionPages *tranPages;
};


#endif
