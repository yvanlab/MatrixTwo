
#ifndef utils_h
#define utils_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
#include "Adafruit_GFX.h"
#include <baseManager.h>


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
    enum TRANSTION_MODE {TRANSITION_LEFT=0, TRANSITION_RIGHT=1, TRANSITION_UP=2,TRANSITION_DOWN=3,
    TRANSITION_FROM_CENTER=4,TRANSITION_ROTATION=5, TRANSITION_STAR=6, TRANSITION_NONE=7 };
    TransitionPages(Window *win, int16_t maxW, int16_t maxH) {
        _win  = win;
        _maxW = maxW;
        _maxH = maxH;
    }

    void startTransition(TRANSTION_MODE mode) {
        _isTransitionMode = true;
        _mode = mode;
        DEBUGLOGF("start transition [%d]\n", _mode);
        if (mode == TRANSITION_NONE) {
            _isTransitionMode = false;
        } else if (_mode == TRANSITION_LEFT) {
            _win->set(_maxW,0,_maxW,_maxH);
        } else if (_mode == TRANSITION_RIGHT) {
            _win->set(-1*_maxW,0,_maxW,_maxH);           
        } else if (_mode == TRANSITION_ROTATION) {
            _tmpInc = 180;
        } else if (_mode == TRANSITION_UP) {
            _win->set(0,_maxH,_maxW,_maxH);           
        } else if (_mode == TRANSITION_DOWN) {
            _win->set(0,-1*_maxH,_maxW,_maxH);           
        } else if (_mode == TRANSITION_FROM_CENTER) {
            _win->set(_maxW/2,_maxH/2,0,0);           
        }

    };
    
    void nextStep() {
        if (!_isTransitionMode) return;

        if (_mode == TRANSITION_LEFT) {
            _tmpInc = _win->_x-2;
            _win->setX(_tmpInc);
        } else if (_mode == TRANSITION_RIGHT) {
            _tmpInc = _win->_x+2;
            _win->setX(_tmpInc);
        }  else if (_mode == TRANSITION_UP) {
            _tmpInc = _win->_y-2;
            _win->setY(_tmpInc);
        } else if (_mode == TRANSITION_DOWN) {
            _tmpInc = _win->_y+2;
            _win->setY(_tmpInc);
        } else if (_mode == TRANSITION_STAR) {
            _tmpInc = _win->_y+2;
            _win->setY(_tmpInc);
        } else if (_mode == TRANSITION_ROTATION) {
            _tmpInc-=20;
            _win->_x = _maxW/4-(_maxW/4)*cos(radians(_tmpInc));
            _win->_y = (_maxH/4)*sin(radians(_tmpInc));            
        }else if (_mode == TRANSITION_FROM_CENTER) {
            _tmpInc = _win->_x - 1;
            _win->_x = _win->_x - 1;
            _win->_w = _win->_w + 2;
            if (_win->_y>0) {
                _win->_y = _win->_y - 1;
                _win->_h = _win->_h + 2;
            }
            //_win->set(_maxW/2,_maxH/2,0,0);           
        }

        if (_tmpInc == 0) {
            _isTransitionMode = false;
            _win->set(0,0,_maxW,_maxH);           
            DEBUGLOGF("End transition [%d]\n", _mode);            
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
#endif