

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

#ifdef ESP8266
   display_ticker.attach(0.002, display_updater);
 #endif


 //vALIDER avec le timerBegin(0, 80, true);
 #ifdef ESP32
  timer = timerBegin(1, 80, true);

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

	    setPage(HOUR_PAGE_ID);
	    displayPage();
}


void MatrixPages::displayPage(){


	Page *pp=smManager->getPage(_numPage);
	if (pp != NULL  && pp->id == TEST_PAGE_ID) {
		displayTestPage();
	
	} else 	if (pp != NULL) {
		displayScreen(pp);
	} else {
		DEBUGLOGF("Display Error %d\n",_numPage);
		Page tp;
		tp.element[0].set(Element::TEXT,2,15,"PROBLEME",Element::SMALL,0,0,255,true);
		displayScreen(&tp);
	}

}


void  MatrixPages::setPage(uint16_t num) {
 DEBUGLOGF("Page %d\n",num);
 if (num==_numPage) return;
  m_display.clearDisplay();
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



String MatrixPages::buildTxt(String txt, String var) {
	String res=txt;
	if (txt.indexOf("{s}")>=0)
		res.replace("{s}",var );
	else
		res += var;
	//DEBUGLOGF ("buildTxt %s,%s = [%s]\n",txt.c_str(), var.c_str(), res.c_str() );
	
	return res;
}

void MatrixPages::displayScreen(Page *page){

	m_display.clearDisplay();
	char tmpString[10];

	for (uint8_t i=0; i<page->nbElement ;i++) {
		//DEBUGLOGF("x[%d] y[%d], Red[%d], Green[%d], Blue[%d]\n",smManager.screen[i].x, smManager.screen[i].y, smManager.screen[i].red, smManager.screen[i].green, smManager.screen[i].blue);
		//DEBUGLOGF("type [%d], txt[%s]\n",smManager.screen[i].type, smManager.screen[i].txt.c_str());
		if (page->element[i].active) {
			switch (page->element[i].font){
			case Element::SMALL:
				m_display.setFont(&TomThumb);
				break;
			case Element::MEDIUM:
				//m_display.setFont(&FreeSerif9pt7b);
				m_display.setFont();
				break;
			case Element::BIG:
				//m_display.setFont(&FreeSerif12pt7b);
				m_display.setFont(&FreeSerif9pt7b);
				break;

			}

			m_display.setCursor(page->element[i].x,page->element[i].y);
			uint16_t color = m_display.color565(page->element[i].red, page->element[i].green,page->element[i].blue);

			m_display.setTextColor(color);
			if (page->element[i].type == Element::TEXT) {
				m_display.print(page->element[i].txt);
			} else if (page->element[i].type == Element::HOUR) {
				m_display.print(buildTxt(page->element[i].txt,NTP.getTimeStr()));
			}else if (page->element[i].type == Element::DATE) {
				m_display.print(buildTxt(page->element[i].txt, NTP.getDateStr()));
			}else if (page->element[i].type == Element::TEMP) {
				sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getAverage());
				m_display.print(buildTxt(page->element[i].txt , tmpString));
			}else if (page->element[i].type == Element::TEMP_MIN) {
				sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getLastMinValue());
				m_display.print(buildTxt(page->element[i].txt, tmpString));
			}else if (page->element[i].type == Element::TEMP_MAX) {
				sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getLastMaxValue());
				m_display.print(buildTxt(page->element[i].txt, tmpString));
			}else if (page->element[i].type == Element::TEMP_TREND) {
				displayTrend(page->element[i].x,page->element[i].y, color,
					bmpMesure->getTemperatureSensor()->getTrend());
				//sprintf(tmpString,"%2.1f",bmpMesure->getTemperatureSensor()->getTrend());
				//m_display.print(tmpString);
			}else if (page->element[i].type == Element::WATCH) {
				uint8_t r = 15;
				if (!page->element[i].txt.isEmpty()){					
					r = page->element[i].txt.toInt();
				}
				displayWatch(page->element[i].x, page->element[i].y, r, color );		
			 } else if (page->element[i].type == Element::BITMAP) {
				String filename = "default.bmp";
				if (!page->element[i].txt.isEmpty()){					
					filename = "/"+page->element[i].txt;
				}
				displayBitmap(page->element[i].x, page->element[i].y, filename, page->element[i].id, page->element[i].isChanged);
				page->element[i].isChanged = false;
			 }
		}
	}
	m_display.showBuffer();
	//DEBUGLOGF("freemem %d\n", ESP.getFreeHeap());
}


void MatrixPages::displayWatch(uint8_t x, uint8_t y, uint8_t r,  uint16_t color ){
	
	double ss = radians(6*second());
	double mn = radians(6*minute());
	double hh = radians(30.0*hourFormat12() + (3.0*minute())/6.0);
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

void MatrixPages::displayTrend(uint8_t x, uint8_t y, uint16_t color, float trend) {
	uint8_t size = 5;
	uint8_t incline = size*0.7;

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


void MatrixPages::displayBitmap(uint8_t x, uint8_t y, String filename, uint8_t id, boolean isChanged) {
	

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
  	
  	uint16_t *buffer = bitmapCache[id]->getBuffer();
	uint16_t counter =0;
 	for (int yy = 0; yy < bitmapCache[id]->getHeight() ; yy++) {
   		for (int xx = 0; xx < bitmapCache[id]->getWidth(); xx++){
     		m_display.drawPixel(xx + x , yy + y,buffer[counter]);
     		counter++;
   		}
 	}

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



/* int imageHeight = 32;
 int imageWidth = 32;
 int counter = 0;
 for (int yy = 0; yy < imageHeight; yy++)
 {
   for (int xx = 0; xx < imageWidth; xx++)
   {
     m_display.drawPixel(xx + 1 , yy + 1, (uint16_t)pgm_read_word(&GIMP_IMAGE_pixel_data[counter]));

     counter+=2;
	 
   }
 }*/

}
