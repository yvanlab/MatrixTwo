

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
  //matrix = _matrix;
	m_myCOLORS[0] = m_display.color565(255, 0, 0);
	m_myCOLORS[1] = m_display.color565(0, 255, 0);
	m_myCOLORS[2] = m_display.color565(0, 0, 255);
	m_myCOLORS[3] = m_display.color565(255, 255, 255);
	m_myCOLORS[4] = m_display.color565(255, 255, 0);
	m_myCOLORS[5] = m_display.color565(0, 255, 255);
	m_myCOLORS[6] = m_display.color565(255, 0, 255);
	m_myCOLORS[7] = m_display.color565(0, 255, 0);
	randomSeed(analogRead(0));
	//m_myCOLORS[8]={myRED,myGREEN,myBLUE,myWHITE,myYELLOW,myCYAN,myMAGENTA,myBLACK};

};

void MatrixPages::begin(){


	   //m_display = new PxMATRIX(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);//, P_E);
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
	    //m_display.setFastUpdate(true);

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

/*#define HOUR_PAGE   0
#define TEMPERATURE_PAGE   1
#define METEO_PAGE   2
#define OPENBAR_PAGE 3
#define NOEL_PAGE    4
#define NEWYEAR_PAGE 5
#define CFG_PAGE     6

#define CUSTOM_0_PAGE  20
#define CUSTOM_1_PAGE  21
#define CUSTOM_2_PAGE  22


#define ETEINDRE_PAGE 128*/

void MatrixPages::displayPage(){


	Page *pp=smManager->getPage(page);
	if (pp != NULL  && pp->id == TEST_PAGE_ID) {
		displayTestPage();
	
	} else 	if (pp != NULL) {
		displayScreen(pp);
	} else {
		DEBUGLOGF("Display Error %d\n",page);
		Page tp;
		tp.element[0].set(Element::TEXT,2,15,"PROBLEME",Element::SMALL,0,0,255,true);
		displayScreen(&tp);
	}



	/*Page pp;
	switch (page) {
	case  HOUR_PAGE_ID :
		displayHourPage();
		break;
	case  TEMPERATURE_PAGE_ID :

		pp.element[0].set(Element::TEXT,2,15,"Temperature",Element::SMALL,0,0,255,true);
		displayScreen(&pp);
		break;
	case  METEO_PAGE_ID :

		pp.element[0].set(Element::TEXT,2,15,"Meteo",Element::SMALL,0,0,255,true);
		displayScreen(&pp);
		break;
	case  OPENBAR_PAGE_ID :
		displayOpenBar();
		break;
	case  NOEL_PAGE_ID :
		displayJoyeuxNoel();
		break;
	case  NEWYEAR_PAGE_ID :
		displayBonneAnnee();
		break;
	case  CFG_PAGE_ID :
		displayCfgPage();
		break;
	case  CUSTOM_0_PAGE_ID :
		displayScreen(&smManager->page[0]);
		break;
	case  CUSTOM_1_PAGE_ID :
		displayScreen(&smManager->page[1]);
		break;
	case  CUSTOM_2_PAGE_ID:
		displayScreen(&smManager->page[2]);
		break;
	case ETEINDRE_PAGE_ID:
		displayStopPage();
		break;
	default:
		pp.element[0].set(Element::TEXT,2,15,"No idea",Element::MEDIUM,0,0,255,true);
		displayScreen(&pp);
		break;
	}*/
}


void  MatrixPages::setPage(uint16_t num) {
 DEBUGLOGF("Page %d\n",num);
 if (num==page) return;
  m_display.clearDisplay();
  startTransition = 128;
  page=num;
};

void MatrixPages::nextPage() {
	DEBUGLOGF("NextPage %d",page);
  //if (startTransition != 0 ) return;
  uint8_t newpage = (page+1) % 2;
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


void MatrixPages::displayMessagePage(){
	m_display.clearDisplay();
	m_display.setFont();
	m_display.setTextColor(m_display.color565(0, 255, 255));
	m_display.setCursor(2,0);
	m_display.print("BO");//wfManager->getHourManager()->toDTString(STD_TEXT).c_str());
	//m_display.setFont(&FreeSerif9pt7b);
	//m_display.setFont(&TomThumb);
	m_display.setCursor(2,12);
	m_display.setTextColor(m_display.color565(255, 0, 0));
	m_display.print(wfManager->getHourManager()->toDTString(STD_TEXT).c_str());
	m_display.setTextColor(m_display.color565(255, 0, 255));
	m_display.setCursor(2,20);
	m_display.print("BNext");//wfManager->getHourManager()->toDTString(STD_TEXT).c_str());

 /* if (manageTransition(FROM_CENTER) || mtTimer.is1SPeriod()) {
     matrix.drawText(0,0,wfManager->getHourManager()->toDTString(STD_TEXT).c_str(),font4_6, PC_BRIGHT);
     String stemp = periferic->getExtTemp() + "d-" + periferic->getInstantCurrent1() + "A";
     matrix.drawText(84,0,stemp.c_str(),font4_6, PC_BRIGHT);

   }
 if (mtTimer.is25MSPeriod()) {
   //char buffer[50];
   strcpy_P(buffer, (char*)pgm_read_word(&(weatherString[periferic->trendMeteo_meteoVmc])));

   String weather =  periferic->getExtTemp() +
      "C, Max " + periferic->getExtTempMax() +
      "C, Min " + periferic->getExtTempMin() +
      " - " +   FPSTR (weatherString[periferic->getTrendMeteo()]);
   //DEBUGLOGF("weather : %s\n",weather.c_str());
   String messageToDisplay = String(smManager->m_textToDisplay) + " - " + weather;
   //DEBUGLOGF("messageToDisplay : %s\n",messageToDisplay.c_str());
   xDecalage--;
   if (xDecalage < (-8*(int)messageToDisplay.length()))
      xDecalage = 128;
   matrix.drawText(xDecalage,6,messageToDisplay.c_str(),font8_8, PC_BRIGHT);
 }*/
}


void MatrixPages::displayOpenBar(){
		Page pp;
		pp.element[0].set(Element::TEXT,2,12,"Open",Element::BIG,0,0,255,true);
		pp.element[1].set(Element::TEXT,30,16,"bar",Element::BIG,255,255,255,true);
		//pp.nbElement = 2;
		displayScreen(&pp);
	}



void MatrixPages::displayJoyeuxNoel(){

	//DEBUGLOGF("x[%d] y[%d], Red[%d], Green[%d], Blue[%d]\n",smManager->screen[0].x,smManager.screen[0].y,smManager.screen[0].red, smManager.screen[0].green, smManager.screen[0].blue);
	//m_display.clearDisplay();
	Page pp;
	pp.element[0].set(Element::TEXT,0,11,"Joyeux",Element::MEDIUM,255,0,0,true);
	pp.element[1].set(Element::TEXT,25,25,"Noel",Element::MEDIUM,255,255,0,true);
	//pp.nbElement = 2;
	displayScreen(&pp);
}

//static int16_t xDec=0;
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
					filename = page->element[i].txt;
				}
				displayBitmap(page->element[i].x, page->element[i].y, filename, page->element[i].id, page->element[i].isChanged);
				page->element[i].isChanged = false;
			 }
		}
	}
	m_display.showBuffer();
	//DEBUGLOGF("freemem %d\n", ESP.getFreeHeap());
}

void MatrixPages::displayCfgPage(){
	m_display.clearDisplay();

	pp.element[0].set(Element::TEXT,0,10,"Long text that is bigger than the screen",Element::SMALL,255,255,255,true);
	//pp.element[1].set(Element::TEXT,0,12,"Ntp:",Element::SMALL,255,255,255,true);
	//pp.element[2].set(Element::TEXT,0,19,"Bmp:",Element::SMALL,255,255,255,true);
	displayScreen(&pp);
	/*pp.element[0].set(Element::TEXT,30,5,"Net:",Element::SMALL,255,255,255,true);
	pp.element[1].set(Element::TEXT,30,12,"Ntp:",Element::SMALL,255,255,255,true);
	pp.element[2].set(Element::TEXT,30,19,"Bmp:",Element::SMALL,255,255,255,true);
	displayScreen(&pp);*/

}
void MatrixPages::displayCustomPage(){

	m_display.clearDisplay();

	/*for (uint8 i=0; i<smManager.nbObject;i++) {
		//DEBUGLOGF("x[%d] y[%d], Red[%d], Green[%d], Blue[%d]\n",smManager.screen[i].x, smManager.screen[i].y, smManager.screen[i].red, smManager.screen[i].green, smManager.screen[i].blue);
		//DEBUGLOGF("type [%d], txt[%s]\n",smManager.screen[i].type, smManager.screen[i].txt.c_str());
		m_display.setCursor(smManager.screen[i].x,smManager.screen[i].y);
		m_display.setTextColor(m_display.color565(smManager.screen[i].red, smManager.screen[i].green, smManager.screen[i].blue));
		if (smManager.screen[i].type == screenObj::TEXT) {
			m_display.setFont(&FreeSerif9pt7b);
			m_display.print(smManager.screen[i].txt);
		} else if (smManager.screen[i].type == screenObj::WATCH) {
			m_display.print(NTP.getTimeStr());
			//MydrawCircle(smManager.screen[i].x,smManager.screen[i].y, 7, m_display.color565(smManager.screen[i].red, smManager.screen[i].green, smManager.screen[i].blue));
		}
	}*/

}


void  MatrixPages::displayBonneAnnee(){
	m_display.clearDisplay();
	Page pp;
	pp.element[0].set(Element::TEXT,10,23,String(year()),Element::MEDIUM,255,255,255,true);
	pp.element[1].set(Element::TEXT,0,15,"Bonne",Element::MEDIUM,0,0,255,true);
	pp.element[2].set(Element::TEXT,9,28,"Annee",Element::MEDIUM,0,0,255,true);

	displayScreen(&pp);

	m_display.setFont(&TomThumb);

	m_display.setTextColor(m_display.color565(255, 0, 0));
	m_display.setCursor(32,5);
	m_display.print(NTP.getTimeStr());



}

void MatrixPages::displayHourPage(){
	m_display.clearDisplay();
	Page pp;
	pp.element[0].set(Element::TEXT,0,20,NTP.getTimeStr(),Element::MEDIUM,64,64,64,true);
	displayScreen(&pp);

}

// ************************************************
// ** Current : xxKxh
// ** xxA  xxKwh
// *************************************************

void MatrixPages::displayCurrentPage(){
/*  if (manageTransition(FROM_LEFT) || mtTimer.is1MNPeriod()) {
    String ss = "Current " + periferic->getKWH1() + "Kwh" ;
    matrix.drawText(0,0,ss.c_str(),font4_6, PC_BRIGHT);
    ss = periferic->getInstantCurrent1() + "A-" + periferic->getDifferential()+"mA" ;
    matrix.drawText(0,8,ss.c_str(), font8_8, PC_BRIGHT);
  }*/
}

void MatrixPages::displayVMCPage(){
/*  if (manageTransition(FROM_LEFT) || mtTimer.is1MNPeriod()) {
    String ss = "VMC " + periferic->getVMVvts();
    matrix.drawText(0,0,ss.c_str(),font4_6, PC_BRIGHT);
    ss = periferic->getVMCtemp() + "C";
    matrix.drawText(0,8,ss.c_str(), font8_8, PC_BRIGHT);
  }*/
}

void MatrixPages::displayMeteoPage(){
	//if (manageTransition(FROM_RIGHT) || mtTimer.is1MNPeriod()) {
	//m_display.setFont(&TomThumb);
    String ss = "METEO " + String( bmpMesure->getPressionSensor()->getWeatherTrend())
    		+"-"+FPSTR (weatherString[bmpMesure->getPressionSensor()->getWeatherTrend()]);
    m_display.setCursor(2,0);
    m_display.setTextColor(m_display.color565(255, 255, 0));

    ss = String(bmpMesure->getTemperatureSensor()->getValue(),1) + "C,"
    	+ String(bmpMesure->getTemperatureSensor()->getLastMaxValue(),1) + "C,"
		+ String(bmpMesure->getTemperatureSensor()->getLastMinValue(),1) + "C";
    m_display.setCursor(2,16);
    m_display.setTextColor(m_display.color565(255, 0, 0));
    m_display.print(ss.c_str());
  //}
}


String MatrixPages::toString(boolean bJson){
	return "";
}


void MatrixPages::displayWatch(uint8_t x, uint8_t y, uint8_t r,  uint16_t color ){
	
	double ss = radians(6*second());
	double mn = radians(6*minute());
	double hh = radians(30.0*hourFormat12() + (3.0*minute())/6.0);
	double ssTige = r*0.9;
	double hhTige = r*0.7;
	double mnTige = r*0.9;

	/*for (uint8_t h=0;h<360; h++) {
		m_display.drawPixel(x+r*sin(radians(h)),y-r*cos(radians(h)),color );	//1	
	}*/

	m_display.drawCircle(x,y,r,color);
	//m_display.fillCircle(x,y,r,color);
	m_display.drawPixel(x,y+r-1,color); //
	m_display.drawPixel(x,y-r+1,color);
	m_display.drawPixel(x-r+1,y,color);
	m_display.drawPixel(x+r-1,y,color);	
	uint16_t hhColor = m_display.color565(255,255,255);
	for (uint8_t h=0;h<4; h++) {
		float si = r*sin(radians(h*30));
		float co = r*cos(radians(h*30));
		m_display.drawPixel(x+si,y-co,hhColor );	//1	
		m_display.drawPixel(x+si,y+co,hhColor );	//1	
		m_display.drawPixel(x-si,y-co,hhColor );	//1	
		m_display.drawPixel(x-si,y+co,hhColor );	//1	
	}
/*	m_display.drawPixel(x+mnTige*sin(radians(30)),y-mnTige*cos(radians(30)),hhColor );	//1
	m_display.drawPixel(x+mnTige*sin(radians(60)),y-mnTige*cos(radians(60)),hhColor );	//2
	m_display.drawPixel(x+mnTige*sin(radians(120)),y-mnTige*cos(radians(120)),hhColor );	//4
m_display.drawPixel(x+mnTige*sin(radians(120)),y-mnTige*cos(radians(120)),hhColor );	//4	*/


	m_display.drawLine(x,y,x+mnTige*sin(mn),y-mnTige*cos(mn), m_display.color565(0,255,0));
	m_display.drawLine(x,y,x+hhTige*sin(hh),y-hhTige*cos(hh), m_display.color565(255,0,0));
	m_display.drawLine(x,y,x+ssTige*sin(ss),y-ssTige*cos(ss), m_display.color565(255,255,255));

	/*m_display.setCursor(50, 6);
    m_display.print(String(second()).c_str());*/
}

void MatrixPages::displayTrend(uint8_t x, uint8_t y, uint16_t color, float trend) {
	uint8_t size = 5;
	uint8_t incline = size*0.7;
	trend = -1;

	if (trend == 0) {
		m_display.drawLine(x,y,x+size,y, color);	
		m_display.drawLine(x,y+1,x+size,y+1, color);	
		m_display.drawLine(x,y+2,x+size,y+2, color);			
		m_display.drawPixelRGB565(x+size+1,y+1, color);			
		m_display.drawPixelRGB565(x+size-2,y+3, color);			
		m_display.drawPixelRGB565(x+size-2,y-1, color);			
	} else if (trend < 0) {
		m_display.drawLine(x,y,x+size,y+4, color);	
		m_display.drawLine(x,y+1,x+size,y+5, color);	
		m_display.drawLine(x,y+2,x+size,y+6, color);	
		
		m_display.drawLine(x,y,x+incline,y+incline, color);	
		m_display.drawLine(x+1,y+1,x+1+incline,y+1+incline, color);	
		m_display.drawLine(x+2,y+2,x+2+incline,y+2+incline, color);	

		color = m_display.color565(0,00,255);
		m_display.drawLine(x-1+incline,y+incline,x+1+1+incline,y+1-1+incline, color);
		m_display.drawLine(x+2+incline,y+2+1+incline,x+1+1+incline,y+1-1+incline, color);	


	} else {
		m_display.drawLine(x,y,x+incline,y-incline, color);	
		m_display.drawLine(x+1,y+1,x+1+incline,y+1-incline, color);	
		m_display.drawLine(x+2,y+2,x+2+incline,y+2-incline, color);	
		
		//color = m_display.color565(0,00,255);
		m_display.drawLine(x-1+incline,y-incline,x+1+1+incline,y+1-1-incline, color);
		m_display.drawLine(x+2+incline,y+2+1-incline,x+1+1+incline,y+1-1-incline, color);	
		
		/*m_display.drawPixelRGB565(x+size+1,y-3, color);			
		drawPixelRGB565(x+size-2,y-4, color);			
		m_display.drawPixelRGB565(x+size-2,y, color);*/

	}
	/*m_display.setCursor(50, 6);
    m_display.print(String(trend).c_str());*/

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

//#include "bb.h"
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



void MatrixPages::displayStopPage(){

	

}
