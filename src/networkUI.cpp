
#include "main.h"

String getJson()
{
  DEBUGLOG("getJson");
//portENTER_CRITICAL_ISR(&wtimerMux);


  String tt("{\"module\":{");
    tt += "\"nom\":\"" + String(MODULE_NAME) +"\"," ;
    tt += "\"version\":\"" + String(LA_MATRIX2_VERSION) +"\"," ;
    tt += "\"status\":\"" + String(STATUS_PERIPHERIC_WORKING) +"\"," ;
    tt += "\"uptime\":\"" + NTP.getUptimeString() +"\"," ;
    tt += "\"datetime\":{" + wfManager->getHourManager()->toDTString(JSON_TEXT) +"}," ;
    tt += "\"build_date\":\""+ String(__DATE__" " __TIME__)  +"\"},";
    tt += "\"setting\":{" + smManager->toString(JSON_TEXT)  + "},";
    tt += "\"bmp\":{" + bmpMesure->toString(JSON_TEXT)  + "},";
    tt += "\"LOG\":["+ wfManager->log(JSON_TEXT)  + "," + smManager->log(JSON_TEXT)+/*","+bmpMesure->log(JSON_TEXT)+*/"]";
    tt += "}";

    //portEXIT_CRITICAL_ISR(&wtimerMux);
    return tt;
}


void dataJson() {
	digitalWrite ( PIN_LED, LOW );
  wfManager->getServer()->send ( 200, "text/json", getJson() );
  digitalWrite ( PIN_LED, HIGH );
}

void saveConfiguration() {
	//mpPages->stopTimer();
	smManager->writeData();
	//mpPages->startTimer();
	dataPage();
}

void setData(){
  bool changed = false;
  //String str;

  DEBUGLOG("SetData");
#ifdef MCPOC_TEST
  for (uint8_t i =0; i<wfManager->getServer()->args();i++) {
	  DEBUGLOGF("[%s,%s]",wfManager->getServer()->argName(i).c_str(),wfManager->getServer()->arg(i).c_str());
  }
  DEBUGLOG("");
#endif
  uint16_t idPage = 0;
  uint8_t nbElt = 0;
  Page *pp = NULL;

  String str = wfManager->getServer()->arg("freq");
  if (str!=NULL){
	  smManager->displayedFrequence = (uint8_t)atoi(str.c_str());
	  return;
  }
  str = wfManager->getServer()->arg("def");
  if (str!=NULL){
  	  smManager->displayedMode = (uint8_t)atoi(str.c_str());
  	  return;
   }

  str = wfManager->getServer()->arg("page");
  if (str!=NULL){
	  idPage = (uint16_t)atoi(str.c_str());
	  pp = smManager->getPage(idPage);
	  if (pp==NULL) DEBUGLOGF("Not page associated to [%d]", idPage);
  }

  str = wfManager->getServer()->arg("obj");

  if (str!=NULL){
	  nbElt = (uint8_t)atoi(str.c_str());
  }

  str = wfManager->getServer()->arg("displaypage");
  if (str!=NULL){
	  if (str == "true") {
		  mpPages->setPage(idPage);
		  smManager->displayedPage = idPage;
		  pp->active = true;
		  changed = true;
	  }else {
		  pp->active = false;
	  }
  }else{
	  str = wfManager->getServer()->arg("x");
	  if (str!=NULL){
		  pp->element[nbElt].x = (uint8_t)atoi(str.c_str());
		  changed = true;
	  }

	  str = wfManager->getServer()->arg("y");
	  if (str!=NULL) {
		  pp->element[nbElt].y = (uint8_t)atoi(str.c_str());
		  changed = true;
	  }

	  str = wfManager->getServer()->arg("color");
	  if (str!=NULL) {
		  uint32_t col = strtoul(str.c_str(),NULL,16);
		  DEBUGLOGF("color [%s],[%s]\n",String(col,HEX).c_str(), str.c_str());
		  pp->element[nbElt].red = (uint8_t)((col&0xFF0000)>>16);
		  pp->element[nbElt].green = (uint8_t)((col&0x00FF00)>>8);
		  pp->element[nbElt].blue= (uint8_t)(col&0x0000FF);
		  changed = true;
	  }


	  if (wfManager->getServer()->hasArg("txt")){
		  str = wfManager->getServer()->arg("txt");
		  if (str==NULL) {
			  str="";
		  }
		  pp->element[nbElt].txt = str;
		  changed = true;
	  }

	  str = wfManager->getServer()->arg("font");
	  if (str!=NULL) {
		  pp->element[nbElt].font= (Element::FONT_TYPE)atoi(str.c_str());
	  	  changed = true;
	  }

	  str = wfManager->getServer()->arg("type");
	  if (str!=NULL) {
		  pp->element[nbElt].type= (Element::OBJECT_TYPE)atoi(str.c_str());
	  	  changed = true;
	  }
	  str = wfManager->getServer()->arg("active");
	  if (str!=NULL) {
		  pp->element[nbElt].active = str == "true";
 	  	  changed = true;
 	 }
  }


  if ( changed ) {
	mpPages->displayPage();
  }

  dataPage();
}


const char HTML[] PROGMEM =
		"\n<html>\n<head>\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\n<style>\nbody {\n\tfont-family: Arial;\n}\n\n/* Style the tab */\n.tab {\n\toverflow: hidden;\n\tborder: 1px solid #ccc;\n\tbackground-color: #f1f1f1;\n}\n\n/* Style the buttons inside the tab */\n.tab button {\n\tbackground-color: inherit;\n\tfloat: left;\n\tborder: none;\n\toutline: none;\n\tcursor: pointer;\n\tpadding: 14px 16px;\n\ttransition: 0.3s;\n\tfont-size: 17px;\n}\n\n/* Change background color of buttons on hover */\n.tab button:hover {\n\tbackground-color: #ddd;\n}\n\n/* Create an active/current tablink class */\n.tab button.active {\n\tbackground-color: #ccc;\n}\n\n/* Style the tab content */\n.tabcontent {\n\tdisplay: none;\n\tpadding: 6px 12px;\n\tborder: 1px solid #ccc;\n\tborder-top: none;\n}\n</style>\n</head>\n<body>\n\t<p>&nbsp;</p>\n\t<h1 style=\"text-align: center;\">\n\t\t<span id=\"modulename\">Module</span> configuration\n\t</h1>\n\t<div style=\"text-align: center;\">\n\t\t<p>\n\t\t\tVersion : <span id=\"version\">3.0</span>\n\t\t</p>\n\t</div>\n\t<div style=\"text-align: center;\">\n\t\t<p>\n\t\t\tDate build : <span id=\"builddate\">10/08/1900</span>\n\t\t</p>\n\t</div>\n\t<div style=\"text-align: center;\">\n\t\t<p>\n\t\t\tUp time : <span id=\"uptime\">un moment</span>\n\t\t</p>\n\t</div>\n\t<div style=\"text-align: center;\">\n\t\t<p>\n\t\t\t<a href=\"/credential\">Credentials</a>/<a href=\"/reset\">Wifi Reset</a>/<a\n\t\t\t\thref=\"/restart\">Restart</a>\n\t\t</p>\n\t</div>\n\t<hr\n\t\tstyle=\"margin-top: 51px; margin-left: auto; width: 177px; margin-right: auto;\" />\n\t<div>\n\t\t<h3 style=\"height: 3px;\">Temperature</h3>\n\t</div>\n\t<div>\n\t\t<hr />\n\t\t<table style=\"height: 21px; width: 429.6px; float: left;\">\n\t\t\t<tbody>\n\t\t\t\t<tr>\n\t\t\t\t\t<td style=\"width: 101px;\">\n\t\t\t\t\t\t<h1 id=\"temp\" style=\"text-align: center;\">25c</h1>\n\t\t\t\t\t</td>\n\t\t\t\t\t<td style=\"width: 314.6px;\">\n\t\t\t\t\t\t<table style=\"height: 34px; width: 177px; float: left;\">\n\t\t\t\t\t\t\t<tbody>\n\t\t\t\t\t\t\t\t<tr>\n\t\t\t\t\t\t\t\t\t<td style=\"width: 52px; text-align: center;\">min</td>\n\t\t\t\t\t\t\t\t\t<td style=\"width: 52px; text-align: center;\">max</td>\n\t\t\t\t\t\t\t\t\t<td style=\"width: 53px; text-align: center;\">tend</td>\n\t\t\t\t\t\t\t\t</tr>\n\t\t\t\t\t\t\t\t<tr>\n\t\t\t\t\t\t\t\t\t<td id=\"tempMin\" style=\"width: 52px; text-align: center;\">10c</td>\n\t\t\t\t\t\t\t\t\t<td id=\"tempMax\" style=\"width: 52px; text-align: center;\">25c</td>\n\t\t\t\t\t\t\t\t\t<td id=\"tempTrend\" style=\"width: 53px; text-align: center;\">&nbsp;</td>\n\t\t\t\t\t\t\t\t</tr>\n\t\t\t\t\t\t\t</tbody>\n\t\t\t\t\t\t</table>\n\t\t\t\t\t</td>\n\t\t\t\t</tr>\n\t\t\t</tbody>\n\t\t</table>\n\t\t&nbsp;\n\t</div>\n\t<br />\n\t<br />\n\t<div>\n\t\t<h3 style=\"height: 4px; text-align: left\">Prevision</h3>\n\t\t<hr />\n\t</div>\n\t<div>\n\t\t<p>Tendance blablabla</p>\n\t</div>\n\t\n\t<div>\n\t\t<h3 style=\"height: 3px;\">Pages Configuration</h3>\n\t\t<hr />\n\t</div>\n\t<span id=\"lstCfg\">cfg</span>\n<!-- \t<div> -->\n\t\t\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x20,100,'displaypage', this.checked )\" />Heure -->\n<!-- \t</div> -->\n<!-- \t<div> -->\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x1,101,'displaypage', this.checked )\" />Temperature -->\n<!-- \t</div> -->\n<!-- \t<div> -->\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x2,102,'displaypage', this.checked )\" />Meteo -->\n<!-- \t</div> -->\n<!-- \t<div> -->\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x4,104,'displaypage', this.checked )\" />Open -->\n<!-- \t\tBar -->\n<!-- \t</div> -->\n<!-- \t<div> -->\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x8,105,'displaypage', this.checked )\" />Joyeux -->\n<!-- \t\tNoel -->\n<!-- \t</div> -->\n<!-- \t<div> -->\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x10,106,'displaypage', this.checked )\" />Bonne -->\n<!-- \t\tAnnee -->\n<!-- \t</div> -->\n<!-- \t<div> -->\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x40,110,'displaypage', this.checked )\" />Page -->\n<!-- \t\t1 -->\n<!-- \t</div> -->\n<!-- \t<div> -->\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x80,111,'displaypage', this.checked )\" />Page -->\n<!-- \t\t2 -->\n<!-- \t</div> -->\n<!-- \t<div> -->\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x100,112,'displaypage', this.checked )\" />Page -->\n<!-- \t\t3 -->\n<!-- \t</div> -->\n<!-- \t<div> -->\n<!-- \t\t<input type=\"checkbox\" -->\n<!-- \t\t\tonchange=\"changeSetting(0x8000,200,'displaypage', this.checked )\" />Log -->\n<!-- \t</div> -->\n\t<div>\n\t\t<br />\n\t\t<div>\n\t\t\t<input id=\"defID\" type=\"checkbox\" onchange=\"changeSetting(0,0,'def', this.checked )\"  /> defilement \n\t\t\t<input id=\"freqID\" type=\"number\" value=\"10\"  onchange=\"changeSetting(0,0,'freq', this.value )\" \" />\n\t\t</div>\n\t</div>\n\t<hr />\n\n\n\t<h2>\n\t\tTabs\n\t\t<button onclick=\"saveConfiguration()\">Save</button>\n\t</h2>\n\t<p>Click on the buttons inside the tabbed menu:</p>\n\n\t<div class=\"tab\">\n\t\t<button class=\"tablinks\" onclick=\"openPage(event, 'P1')\">Page\n\t\t\t1</button>\n\t\t<button class=\"tablinks\" onclick=\"openPage(event, 'P2')\">Page\n\t\t\t2</button>\n\t\t<button class=\"tablinks\" onclick=\"openPage(event, 'P3')\">Page\n\t\t\t3</button>\n\t</div>\n\n\t<div id=\"P1\" class=\"tabcontent\">\n\t\t<h3>\n\t\t\t<span id=\"namePage0\">Files</span>\n\t\t</h3>\n\t\t<i><b><span id=\"lstPage0\">Files</span></b></i>\n\t</div>\n\n\t<div id=\"P2\" class=\"tabcontent\">\n\t\t<h3>\n\t\t\t<span id=\"namePage1\">Files</span>\n\t\t</h3>\n\t\t<i><b><span id=\"lstPage1\">Files</span></b></i>\n\t</div>\n\n\t<div id=\"P3\" class=\"tabcontent\">\n\t\t<h3>\n\t\t\t<span id=\"namePage2\">Files</span>\n\t\t</h3>\n\t\t<i><b><span id=\"lstPage2\">Files</span></b></i>\n\t</div>\n\n\n\n\t<script>\n\t\t\n\t\t\tfunction saveConfiguration() {\n\t\t\t\tvar xhttp = new XMLHttpRequest();\n\t\t\t\txhttp.onreadystatechange = function() {\n\t\t\t\t\tif (this.readyState == 4 && this.status == 200) {\n\t\t\t\t\t\tupdate(this.responseText)\n\t\t\t\t\t}\n\t\t\t\t};\n\t\n\t\t\t\txhttp.open(\"GET\", \"save\",true);\n\t\t\t\txhttp.send();\t\n\t\t\t}\n\t\t\n\t\t\tfunction openPage(evt, cityName) {\n\t\t\t\tvar i, tabcontent, tablinks;\n\t\t\t\ttabcontent = document.getElementsByClassName(\"tabcontent\");\n\t\t\t\tfor (i = 0; i < tabcontent.length; i++) {\n\t\t\t\t\ttabcontent[i].style.display = \"none\";\n\t\t\t\t}\n\t\t\t\ttablinks = document.getElementsByClassName(\"tablinks\");\n\t\t\t\tfor (i = 0; i < tablinks.length; i++) {\n\t\t\t\t\ttablinks[i].className = tablinks[i].className.replace(\n\t\t\t\t\t\t\t\" active\", \"\");\n\t\t\t\t}\n\t\t\t\tdocument.getElementById(cityName).style.display = \"block\";\n\t\t\t\tevt.currentTarget.className += \" active\";\n\t\t\t}\n\t\t\t\n\t\t\tfunction changeSetting(pID, oID, act, val) {\n\t\t\t\tdocument.getElementById(\"version\").innerHTML = val;\n\t\t\t\tvar xhttp = new XMLHttpRequest();\n\t\t\t\txhttp.onreadystatechange = function() {\n\t\t\t\t\tif (this.readyState == 4 && this.status == 200) {\n\t\t\t\t\t\tupdate(this.responseText)\n\t\t\t\t\t}\n\t\t\t\t};\n\t\t\t\tdocument.getElementById(\"version\").innerHTML = val;\n\t\t\t\txhttp.open(\"GET\", \"setData?obj=\" + oID+ \"&page=\" + pID + \"&\" + act + \"=\" + String(val).replace(\"#\",\"\"),\n\t\t\t\t\t\ttrue);\n\t\t\t\txhttp.send();\n\t\t\t};\n\n\t\t\tfunction update(responseText) {\n\t\t\t\tvar myObj = JSON.parse(responseText);\n\t\t\t\tdocument.getElementById(\"builddate\").innerHTML = myObj.module.build_date;\n\t\t\t\tdocument.getElementById(\"version\").innerHTML = myObj.module.version;\n\t\t\t\tdocument.getElementById(\"modulename\").innerHTML = myObj.module.nom;\n\t\t\t\tdocument.getElementById(\"uptime\").innerHTML = myObj.module.uptime;\n\n\t\t\t\tdocument.getElementById(\"temp\").innerHTML = myObj.bmp.Btemperature.value;\n\t\t\t\tdocument.getElementById(\"tempMax\").innerHTML = myObj.bmp.Btemperature.maxValue;\n\t\t\t\tdocument.getElementById(\"tempMin\").innerHTML = myObj.bmp.Btemperature.minValue;\n\t\t\t\tdocument.getElementById(\"tempTrend\").innerHTML = myObj.bmp.Btemperature.trend;\n\t\t\t\t\n\t\t\t\tdocument.getElementById(\"defID\").innerHTML = myObj.setting.displayedMode;\n\t\t\t\tdocument.getElementById(\"freqID\").innerHTML = myObj.setting.displayedFreq;\n\t\t\t\t\n\t\t\t\t\n\t\t\t\t/* Build cfg */\n\t\t\t\tvar res = \"\";\n\t\t\t\tvar tmp=\"\";\n\t\t\t\tvar page ;\n\t\t\t\tres = buildDisplayPages(myObj.setting.predefpage );\n\t\t\t\tres += buildDisplayPages(myObj.setting.page );\n\t\t\t\tdocument.getElementById(\"lstCfg\").innerHTML = res;\n\t\t\t\t\n\t\t\t\tfor (i in myObj.setting.page) {\n\t\t\t\t \tpage = myObj.setting.page[i];\n\t\t\t\t\tvar pName = \"Page\"+ i;\n\t\t\t\t\tdocument.getElementById(\"name\"+pName).innerHTML = page.name;\n\t\t\t\t\tres = \"\";\n\t\t\t\t\ttmp=\"\";\n\t\t\t\t\tvar lstType = [\"Text\",\"Heure\",\"Date\",\"Temperature\",\"Temperature Min\", \"Temperature Max\", \"Tendances\", \"je ne sais pas\"];\n\t\t\t\t\tvar lstFont = [\"Petite\",\"Moyenne\",\"Grande\"];\n\t\t\t\t\tfor (j in page.obj) {\n\t\t\t\t\t\tvar ob = page.obj[j];\n\t\t\t\t\t\tres += '<hr />';\n\t\t\t\t\t\tres += ob.id\n\t\t\t\t\t\tif (ob.active==1)tmp=\"checked \"; else tmp=\" \";\n\t\t\t\t\t\tres += '<input type=\"checkbox\" ' + tmp + buildOnChangeSettingCheck( page.id, ob.id, \"active\") + '\\>';\n\t\t\t\t\t\tres += 'Type:<select name=\"type\"' +  buildOnChangeSetting( page.id, ob.id, \"type\") + '\\>';\n\t\t\t\t\t\tres += buildDropdown(ob.type,lstType);\n\t\t\t\t\t\tres += '</select>';\n\t\t\t\t\t\tres += 'Font:<select name=\"type\"' +  buildOnChangeSetting( page.id, ob.id, \"font\") + '\\>';\n\t\t\t\t\t\tres += buildDropdown(ob.font,lstFont);\n\t\t\t\t\t\tres += '</select>';\n\t\t\t\t\t\tres += 'Couleur<input type=\"color\" value=\"' + ob.color + '\"'\n\t\t\t\t\t\t\t+ buildOnChangeSetting(page.id, ob.id, \"color\") + '\\>';\n\t\t\t\t\t\t\n\t\t\t\t\t\tres += '<div>X:<input type=\"number\" style = \"padding-left:6px;width:60px;\" value=' + ob.x\n\t\t\t\t\t\t \t\t+ buildOnChangeSetting(page.id, ob.id, \"x\") + '\\>';\n\t\t\t\t\t\t\n\t\t\t\t\t\tres += 'Y:<input type=\"number\" style = \"padding-left:6px;width:60px;\" value=' + ob.y\n\t\t\t\t\t\t\t\t+ buildOnChangeSetting(page.id, ob.id, \"y\") + '\\>';\n\t\t\n\t\t\t\t\t\tres += '<br />Text<input type=\"edit\" value=' + ob.txt\n\t\t\t\t\t\t\t\t+ buildOnChangeSetting(page.id, ob.id, \"txt\") + '\\>';\n\t\t\t\t\t};\n\t\t\t\t\tdocument.getElementById(\"lst\"+pName).innerHTML = res;\n\t\t\t\t};\n\t\t\t};\n\t\t\t\n\t\t\t\n\t\t\tfunction buildDisplayPages(lstPages ) {\n\t\t\t\tvar res = \"\";\n\t\t\t\tvar tmp=\"\";\n\t\t\t\tvar page ;\n\t\t\t\tfor (i in lstPages) {\t\t\t\t\t\n\t\t\t\t\tpage = lstPages[i];\t\t\t\t\t\n\t\t\t\t\tif (page.active==1)tmp=\"checked \"; else tmp=\" \";\n\t\t\t\t\tres += '<input type=\"checkbox\" ' + tmp + buildOnChangeSettingCheck( page.id, 100, \"displaypage\")+'\\>' + page.name + '<br>';\n\t\t\t\t}\n\t\t\t\treturn res;\n\t\t\t}\n\t\t\t\n\t\t\tfunction buildOnChangeSettingCheck(pID, oID,oTXT) {\n\t\t\t\tvar res = ' onchange=\"changeSetting('\n\t\t\t\t\t+ pID + ',' + oID + ',\\'' + oTXT + '\\',this.checked)\"';\n\t\t\t\treturn res;\n\t\t\t}\n\t\t\t\n\t\t\tfunction buildOnChangeSetting(pID, oID,oTXT) {\n\t\t\t\tvar res = ' onchange=\"changeSetting('\n\t\t\t\t\t+ pID + ',' + oID + ',\\'' + oTXT + '\\',this.value)\"';\n\t\t\t\treturn res;\n\t\t\t}\n\t\t\t\n\t\t\tfunction buildDropdown(sel,lstOption) {\n\t\t\t\t\n\t\t\t\t\n\t\t\t\tvar res = \"\";\n\t\t\t\tfor (i in lstOption) {\n\t\t\t\t\tvar selected=\" \";\n\t\t\t\t\tif (i==sel) selected=\" selected\";\n\t\t\t\t\tres += '<option value='+ i + selected +' >' + lstOption[i] + '</option>';\n\t\t\t\t};\n\t\t\t\t\n\t\t\t  return res\n\t\t\t}\n\n\t\t\tfunction init() {\n\t\t\t\tvar xhttp = new XMLHttpRequest();\n\t\t\t\txhttp.onreadystatechange = function() {\n\t\t\t\t\tif (this.readyState == 4 && this.status == 200) {\n\t\t\t\t\t\tupdate(this.responseText)\n\t\t\t\t\t}\n\t\t\t\t};\n\n\t\t\t\txhttp.open(\"GET\", \"status\", true);\n\t\t\t\txhttp.send();\n\t\t\t}\n\t\t\twindow.onload = init;\n\t\t</script>\n\n</body>\n</html>\n"
		;


void dataPage() {
	digitalWrite ( PIN_LED, LOW );

	DEBUGLOG("dataPage");

	wfManager->getServer()->send_P ( 200, "text/html", HTML );

	digitalWrite ( PIN_LED, HIGH );


}

