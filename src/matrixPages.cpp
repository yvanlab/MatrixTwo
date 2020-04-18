

#include "matrixPages.h"

//#include "myTimer.h"
//#include "settingManager.h"

#include "main.h"
#include "BMPFile.h"

#include "constString.h"

#include <PxMatrix.h>
//#include <Fonts/FreeMonoBoldOblique9pt7b.h>
//#include <Fonts/FreeSerifBold24pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
//#include <Fonts/FreeSerif12pt7b.h>
//#include <Fonts/Org_01.h>
#include <Fonts/TomThumb.h>

/*#include "font/font8x8_basic.h"
#include "font/font8x16_basic.h"
#include "font/font4x6_basic.h"*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef ESP8266
// ISR for display refresh
void MatrixPages::display_updater()
{
	m_display.display(70);
}
#endif

#ifdef ESP32
TaskHandle_t xTask1;
//portMUX_TYPE wtimerMux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR MatrixPages::display_updater(){
  // Increment the counter and set the time of ISR
	//portENTER_CRITICAL_ISR(&wtimerMux);
	m_display.display(10);
	//displayTrigger = true;
	//DEBUGLOG("i");
	//portEXIT_CRITICAL_ISR(&wtimerMux);
}
#endif



MatrixPages::MatrixPages(unsigned char pinLed) : BaseManager(pinLed){

};

void MatrixPages::begin(){

	m_display.begin(16);
	m_display.flushDisplay();
	m_display.clearDisplay();

	unsigned long start_timer = micros();
	m_display.drawPixel(1, 1, 0);
	unsigned long delta_timer = micros() - start_timer;
	DEBUGLOGF("Pixel draw latency in us: %ld\n",delta_timer);

	start_timer = micros();
	m_display.display(0);
	delta_timer = micros() - start_timer;
	DEBUGLOGF("Display update latency in us: %ld\n",delta_timer);
	m_display.setFlip(true);

	tranPages = new TransitionPages(&win, m_display.width(), m_display.height());
	//m_display.setRotate(true);
	//m_display.setRotation(2);


#ifdef ESP8266
   display_ticker.attach(0.002, display_updater);
 #endif

 #ifdef ESP32
  timer = timerBegin(MATRIX_TIMER_NUM, 80, true);

   //timerAttachInterrupt(timer, &setDisplayTrigger, true);
   timerAttachInterrupt(timer, &MatrixPages::display_updater, true);
   //timerAlarmWrite(timer, 2000, true);
   timerAlarmWrite(timer, 5000, true);   
   timerAlarmEnable(timer);
 #endif

	    //yield();

	    m_display.setFastUpdate(false);

	    m_display.fillRect(0,0,64,32,m_display.color565(0, 0, 0));
	    m_display.setTextColor( m_display.color565(255, 0, 0));
	    m_display.setCursor(2,0);
	    m_display.print("Pixel");

	win.set(0,0,64,32);

	    setPage(HOUR_PAGE_ID);
	    displayPage();
}


void MatrixPages::displayPage(){

	Page *pp=smManager->getPage(_numPage);
	m_display.clearDisplay();
	if (pp == NULL) { 
		DEBUGLOGF("Display Error %d\n",_numPage);
		Page tp;
		tp.element[0].set(Element::TEXT,2,15,"Page not found ",Element::SMALL,255,0,0,true);
		displayScreen(&tp);
		m_display.showBuffer();
		return;
	}
	if (pp->id == TEST_PAGE_ID) {
		displayTestPage();
	} else if (pp->id == CFG_PAGE_ID) {
		displayCfgPage();
	} else{
		displayScreen(pp);
	}
	m_display.showBuffer();
}


void  MatrixPages::setPage(uint16_t num) {
 DEBUGLOGF("Page %d\n",num);
 if (num==_numPage) return;
  m_display.clearDisplay();
  tranPages->startTransition(TransitionPages::TRANSITION_ROTATION);
  startTransition = 128;
  _numPage=num;
};

void MatrixPages::nextPage() {
	DEBUGLOGF("NextPage %d",_numPage);
  //if (startTransition != 0 ) return;
  uint8_t newpage = (_numPage+1) % 2;
  setPage(newpage);
}

boolean MatrixPages::manageTransition(byte transitionType) {
 /* if (startTransition == 0) return false;
  if (!mtTimer.isQuickPeriod()) return false;
  startTransition--;
  if (startTransition==0) {
    matrix.clear();
    return true;
  }

  if (transitionType == FROM_CENTER) {
    matrix.setDisplayWindows(startTransition/2, startTransition/8,128-startTransition, 16-startTransition/8);
    matrix.drawSquare(0,0,startTransition/8-1,16-startTransition/8-1,0);
  }else if (transitionType == FROM_RIGHT) {
    matrix.setDisplayWindows(startTransition, 0,128, 16);
  }else   if (transitionType == FROM_LEFT) {
    matrix.setDisplayWindows(startTransition, 0,128, 16);
  }*/
	 return true;
}

String MatrixPages::buildGeneric(String txt) {
	char tmpString[20];

	//{h-full heure},{hh-heure},{hm-minute},{hs-second} {d-full date}
	txt.replace("{h}",NTP.getTimeStr() );	
	txt.replace("{d}",NTP.getDateStr() );	
	if (txt.indexOf("{y}")>=0) {
		txt.replace("{y}",String(year()) );	
	}
	if (txt.indexOf("{hh}")>=0) {
		txt.replace("{hh}",String(hour()) );	
	}
	if (txt.indexOf("{hm}")>=0) {
		txt.replace("{hm}",String(minute()) );	
	}
	if (txt.indexOf("{hs}")>=0) {
		txt.replace("{hs}",String(second()) );	
	}
	//{t-temperature},{tM-temp Max},{tm-temp min},{tt - Temp tendance},
	if (txt.indexOf("{t}")>=0) {
		sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getLastValue());
		txt.replace("{t}",tmpString );	
	}
	if (txt.indexOf("{tM}")>=0) {
		sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getLastMaxValue());
		txt.replace("{tM}",tmpString );	
	}
	if (txt.indexOf("{tm}")>=0) {
		sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getLastMinValue());
		txt.replace("{tm}",tmpString );	
	}
	if (txt.indexOf("{tt}")>=0) {
		sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getTrend());
		txt.replace("{tt}",tmpString );	
	}
	//{mp-prevision pression},{mt-prevision text},{mi-prevision icon}
	if (txt.indexOf("{mp}")>=0) {
		sprintf(tmpString,"%d",bmpMesure->getPressionSensor()->getWeatherTrend());
		txt.replace("{mp}",tmpString );	
	}
	if (txt.indexOf("{mt}")>=0) {
//		sprintf(tmpString,"%d",bmpMesure->getPressionSensor()->getWeatherTrend());
		txt.replace("{mt}",weatherString[bmpMesure->getPressionSensor()->getWeatherTrend()] );	
	}

	//{im-memoire},{id-duree on},{it-internal temperature}, 
	//{im-detecteur mouvement}, {iv-version}
	if (txt.indexOf("{im}")>=0) {
		txt.replace("{im}",String(ESP.getFreeHeap()/1000));	
	}
	if (txt.indexOf("{id}")>=0) {
		txt.replace("{id}",String(NTP.getUptimeString()));	
	}
	if (txt.indexOf("{it}")>=0) {
		txt.replace("{it}",String((temprature_sens_read() - 32) / 1.8));	
	}
	if (txt.indexOf("{ip}")>=0) {
		txt.replace("{ip}",String((phPresence->getInput())));	
	}
	if (txt.indexOf("{iv}")>=0) {
		txt.replace("{iv}",String(LA_MATRIX2_VERSION));	
	}
	if (txt.indexOf("{ipd}")>=0) {
		txt.replace("{ipd}",String(phPresence->getRemaining()));	
	}
	//DEBUGLOGF ("buildGeneric %s\n",txt.c_str() );
	return txt;
}


String MatrixPages::buildTxt(String txt, String var) {
	//{t-temperature},{tM-temp Max},{tm-temp min},{tt - Temp tendqnce},
	//{mp-prevision pression},{mt-prevision text},{mi-prevision icon}
	//{h-heure},{d-date}
	//{mm-memoire},{dd-duree on}, {detecteur}


	String res=txt;
	if (txt.indexOf("{s}")>=0)
		res.replace("{s}",var );
	else
		res += var;
	//DEBUGLOGF ("buildTxt %s,%s = [%s]\n",txt.c_str(), var.c_str(), res.c_str() );
	
	return res;
}

void MatrixPages::displayScreen(Page *page){


	char tmpString[20];

	tranPages->nextStep();

	for (uint8_t i=0; i<page->nbElement ;i++) {
		//DEBUGLOGF("x[%d] y[%d], Red[%d], Green[%d], Blue[%d]\n",smManager.screen[i].x, smManager.screen[i].y, smManager.screen[i].red, smManager.screen[i].green, smManager.screen[i].blue);
		//DEBUGLOGF("type [%d], txt[%s]\n",smManager.screen[i].type, smManager.screen[i].txt.c_str());
		if (page->element[i].active) {
			switch (page->element[i].font){
			case Element::SMALL:
				m_display.setFont(&TomThumb);
				m_gfxFont.set((GFXfont *)&TomThumb);
				break;
			case Element::MEDIUM:
				//m_display.setFont(&FreeSerif9pt7b);
				m_display.setFont();
				m_gfxFont.set();				
				break;
			case Element::BIG:
				//m_display.setFont(&FreeSerif12pt7b);
				m_display.setFont(&FreeSerif9pt7b);
				m_gfxFont.set((GFXfont *)&FreeSerif9pt7b);
				break;

			}

			m_display.setCursor(win.getX(page->element[i].x),win.getY(page->element[i].y));
			//uint16_t color = m_display.color565(page->element[i].red, page->element[i].green,page->element[i].blue);

			//m_display.setTextColor(color);
			if (page->element[i].type == Element::TEXT) {
				print(&page->element[i],page->element[i].txt );
			} else if (page->element[i].type == Element::HOUR) {
				print(&page->element[i],buildTxt(page->element[i].txt,NTP.getTimeStr()));
			}else if (page->element[i].type == Element::DATE) {
				print(&page->element[i],buildTxt(page->element[i].txt, NTP.getDateStr()));
			}else if (page->element[i].type == Element::TEMP) {
				sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getLastValue());
				print(&page->element[i],buildTxt(page->element[i].txt , tmpString));
			}else if (page->element[i].type == Element::TEMP_MIN) {
				sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getLastMinValue());
				print(&page->element[i],buildTxt(page->element[i].txt, tmpString));
			}else if (page->element[i].type == Element::TEMP_MAX) {
				sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getLastMaxValue());
				print(&page->element[i],buildTxt(page->element[i].txt, tmpString));
			}else if (page->element[i].type == Element::TEMP_TREND) {
				displayTrend(&page->element[i], bmpMesure->getTemperatureSensor()->getTrend());			
			}else if (page->element[i].type == Element::METEO_PRESSION) {
				sprintf(tmpString,"%d",(int)bmpMesure->getPressionSensor()->getLastValue());
				print(&page->element[i],buildTxt(page->element[i].txt, tmpString));
			}else if (page->element[i].type == Element::METEO_TEXT) {
				print(&page->element[i],weatherString[bmpMesure->getPressionSensor()->getWeatherTrend()]);
			}else if (page->element[i].type == Element::METEO_ICON) {
				sprintf(tmpString,"Ic%d",bmpMesure->getPressionSensor()->getWeatherTrend());
				print(&page->element[i],buildTxt(page->element[i].txt, tmpString));
} 			else if (page->element[i].type == Element::GENERIC_TEXT) {
				print(&page->element[i],buildGeneric(page->element[i].txt));
			}else if (page->element[i].type == Element::WATCH) {
				displayWatch(&page->element[i]);		
			 } else if (page->element[i].type == Element::BITMAP) {
				displayBitmap(&page->element[i]);
				page->element[i].isChanged = false;
			 }
		}
	}
	//DEBUGLOGF("freemem %d\n", ESP.getFreeHeap());
}

void MatrixPages::print(Element *pElt, String txt) {

	uint8_t  nbDisplayedChar = 0;
    uint16_t color 	= m_display.color565(pElt->red, pElt->green,pElt->blue);
	int16_t  x  	= win.getX(pElt->x) + pElt->xDec;
	int16_t  xa 	= 0;
	
	//DEBUGLOGF("X[%d],xDec[%d]\n",pElt->x,pElt->xDec);
	for (uint8_t i=0; i<txt.length(); i++ ) {
		char c = txt[i];
		xa = m_gfxFont.getXadvance(c);
		if (xa>0) { // Char present in this font?
			//DEBUGLOGF("C[%c],x[%d],xa[%d]",c,x,xa);
			if (x < pElt->x) {
				x += xa;
				//DEBUGLOG(" - Before X\n");
			}else if ((x + xa )<=(win.getW()+xa) ) {
				m_display.drawChar(x, win.getY(pElt->y), c, color,color, 1);
				//DEBUGLOG(" - Displayed\n");
				x += xa;
				nbDisplayedChar++;
			}else {
				//DEBUGLOG(" - No Display\n");
				break;
			}
		}
	}
	if (!tranPages->isTransition()) {
		if (nbDisplayedChar==0) {
			pElt->xDec = win.getW()-(win.getX(pElt->x)+xa+2);
		} else if (nbDisplayedChar < txt.length()) {
			pElt->xDec -= 1;
		} else {
			pElt->xDec = 0;
		}
	}

}



void MatrixPages::displayWatch(Element *pElt){
	
	double ss = radians(6*second());
	double mn = radians(6*minute());
	double hh = radians(30.0*hourFormat12() + (3.0*minute())/6.0);

	int16_t x = win.getX(pElt->x);
	int16_t y = win.getY(pElt->y);
	uint16_t color = m_display.color565(pElt->red, pElt->green, pElt->blue);
	uint8_t r = 15;
	if (!pElt->txt.isEmpty()){					
		r = pElt->txt.toInt();
	}
	double ssTige = r*0.9;
	double hhTige = r*0.7;
	double mnTige = r*0.9;



	m_display.drawCircle(x,y,r,color);

	uint16_t hhColor = m_display.color565(255,255,255);
	m_display.drawPixel(x,y+r-1,color); //
	m_display.drawPixel(x,y-r+1,color);
	m_display.drawPixel(x-r+1,y,color);
	m_display.drawPixel(x+r-1,y,color);	
	for (uint8_t h=0;h<4; h++) {
		float si = r*sin(radians(h*30));
		float co = r*cos(radians(h*30));
		m_display.drawPixel(x+si,y-co,hhColor );	//1	
		m_display.drawPixel(x+si,y+co,hhColor );	//1	
		m_display.drawPixel(x-si,y-co,hhColor );	//1	
		m_display.drawPixel(x-si,y+co,hhColor );	//1	
	}

	m_display.drawLine(x,y,x+mnTige*sin(mn),y-mnTige*cos(mn), m_display.color565(0,255,0));
	m_display.drawLine(x,y,x+hhTige*sin(hh),y-hhTige*cos(hh), m_display.color565(255,0,0));
	m_display.drawLine(x,y,x+ssTige*sin(ss),y-ssTige*cos(ss), m_display.color565(255,255,255));
}

void MatrixPages::displayTrend(Element *pElt, float trend) {
	uint8_t size = 5;
	uint8_t incline = size*0.7;
	int16_t x = win.getX(pElt->x);
	int16_t y = win.getY(pElt->y);
	uint16_t color = m_display.color565(pElt->red, pElt->green, pElt->blue);

	if (trend == 0) {
		m_display.drawLine(x,y  ,x+size,y, color);	
		m_display.drawLine(x,y+1,x+size,y+1, color);	
		m_display.drawLine(x,y+2,x+size,y+2, color);			
		m_display.drawPixelRGB565(x+size+1,y+1, color);			
		m_display.drawPixelRGB565(x+size-2,y+3, color);			
		m_display.drawPixelRGB565(x+size-2,y-1, color);			
	} else if (trend < 0) {
		y = y - 2;
		color = m_display.color565(0,00,255);
		m_display.drawLine(x,y    ,x+incline  ,y+incline, color);	
		m_display.drawLine(x-1,y+1,x-1+incline,y+1+incline, color);	
		m_display.drawLine(x-2,y+2,x-2+incline,y+2+incline, color);	
		m_display.drawLine(x+incline  ,y+incline-1,x+incline,y+incline+2, color);
		m_display.drawLine(x-2+incline-1,y+2+incline,x+incline,y+incline+2, color);	
	} else {
		color = m_display.color565(255,00,0);
		y = y + 2;
		m_display.drawLine(x,y    ,x+incline  ,y-incline, color);	
		m_display.drawLine(x+1,y+1,x+1+incline,y+1-incline, color);	
		m_display.drawLine(x+2,y+2,x+2+incline,y+2-incline, color);	
		m_display.drawLine(x-1+incline,y-incline,x+1+1+incline,y+1-1-incline, color);
		m_display.drawLine(x+2+incline,y+2+1-incline,x+1+1+incline,y+1-1-incline, color);	

	}

}


void MatrixPages::displayBitmap(Element *pElt) {
	

	String filename = "/tst.bmp";
	if (!pElt->txt.isEmpty()){					
		filename = "/"+pElt->txt;
	}
	int16_t x = win.getX(pElt->x);
	int16_t y = win.getY(pElt->y);
	//uint16_t color = m_display.color565(pElt->red, pElt->green, pElt->blue);
	boolean isChanged = pElt->isChanged;
	uint8_t id = pElt->id;


	if (isChanged &&  bitmapCache[id] != NULL) {
		delete bitmapCache[id];
		bitmapCache[id] = NULL;
		DEBUGLOGF("displayBitmap delete [%d]", id);
	}
	
	if (bitmapCache[id] == NULL) {
		stopTimer();
		bitmapCache[id] = new BMPFile;
		if (bitmapCache[id]->loadBitmap(filename) != 0) {
			delete bitmapCache[id];
			bitmapCache[id] = NULL;
			startTimer();		
			return;
		}
		startTimer();				
	}
  	
  	
	m_display.drawRGBBitmap(x,y,bitmapCache[id]->getBuffer(),
                    bitmapCache[id]->getWidth(), bitmapCache[id]->getHeight());
}   

void MatrixPages::displayCfgPage() {
	Page *pp=smManager->getPage(CFG_PAGE_ID);
	displayScreen(pp);

	String sPres;
	if (phPresence->getInput()) {
		sPres = "On";
	} else  {
		sPres = "Off";
	}
	//freemem
	//last reboot
	Page tp;
	tp.element[0].set(Element::TEXT,win.getX(31),win.getY(19),sPres,Element::SMALL,0,0,255,true);
	tp.element[1].set(Element::METEO_PRESSION,win.getX(31),win.getY(27),"",Element::SMALL,0,255,0,true);
	tp.element[2].set(Element::METEO_ICON,win.getX(53),win.getY(19),"",Element::SMALL,0,255,0,true);	
	displayScreen(&tp);

}


void MatrixPages::displayTestPage() {


stopTimer();

  BMPFile ff;
  ff.loadBitmap("/bb.bmp");
  uint16_t *buffer = ff.getBuffer();
  
/*  ff = SPIFFS.open("/bb.bmp", "r");
  bb.loadImageHeader();
  bb.loadImage565(buffer);
  ff.close();*/
startTimer();
  uint16_t counter =0;
 for (int yy = 0; yy < ff.getHeight() ; yy++)
 {
   for (int xx = 0; xx < ff.getWidth(); xx++)
   {
     m_display.drawPixel(xx + 1 , yy + 1,buffer[counter]);
     counter++;
   }
 }
}
