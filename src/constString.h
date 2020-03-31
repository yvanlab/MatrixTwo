

 #ifndef __CONSTSTRING_H__
 #define __CONSTSTRING_H__
#include <pgmspace.h>

/*
   #define BPM_TEMPS_BEAU     1 //:   > 1020               :  Beau ou assez beau. Journées chaudes, nuits fraîches.
   #define BPM_TEMPS_ORAGEUX  2 //: De 1013 à 1020 hPa	  :  Assez beau ou ondées orageuses. Journées chaudes, nuits un peu fraîches.
   #define BPM_TEMPS_PLUIE    3 //: De 1006 à 1013 hPa   :	 Pluie ou averses avec un peu de vent. Temps doux.
   #define BPM_TEMPS_TEMPETE  4 //:  < 1006 hPa	          :  Pluie et vent assez fort. Températures douces.
   */
   //static const char xyz[] PROGMEM = "pas de prevision";
   static const char string_0[] PROGMEM = "pas de prevision";   // "String 0" etc are strings to store - change to suit.
   static const char string_1[] PROGMEM = "Beau ou assez beau";
   static const char string_2[] PROGMEM = "Assez beau ou ondees orageuses";
   static const char string_3[] PROGMEM = "Pluie ou averses avec un peu de vent";
   static const char string_4[] PROGMEM = "Pluie et vent assez fort";

   static const char *weatherString[]  =  {string_0, string_1, string_2, string_3, string_4};
//const char* const weatherString[] PROGMEM = {string_0, string_1, string_2, string_3, string_4};


#endif
