
#ifndef prog_h
#define prog_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif
//#include "Adafruit_GFX.h"
#include <baseManager.h>
#include "SPIFFS.h"
#include <SD.h>



#define maxPrgElt 10
const char prgFileName[] = "/prog.json";

class ProgElt
{
public:
    int16_t pgId = -1;
    String hourMinute;
    int16_t hourMinuteConverted = 32500;
    String pgName;
    String toString(boolean bJson)
    {
        String ss;
        ss += "{\"pgId\":\"" + String(pgId) + "\",";
        ss += "\"pgName\":\"" + pgName + "\",";
        ss += "\"hour\":\"" + hourMinute + "\"}";
        return ss;
    }
    void fromJson(JsonObject doc)
    {
        pgName = doc[F("pgName")].as<String>();
        pgId = doc[F("pgId")];
        setHourMinute(doc[F("hour")].as<String>());
    }

    void setHourMinute(String hhMm)
    {
        //DEBUGLOGF("setHourMinute [%s][%s]\n",name.c_str(), hhMm.c_str());
        if (!hhMm.isEmpty() && hhMm.length() == 5)
        {
            //22:12
            uint16_t hh = hhMm.substring(0, 2).toInt();
            uint16_t mn = hhMm.substring(3, 5).toInt();
            DEBUGLOGF("setHourMinute [%s][%s],[%d][%d]\n", pgName.c_str(), hhMm.c_str(), hh, mn);
            hourMinuteConverted = hh * 60 + mn;
            hourMinute = hhMm;
        }
        else
        {
            hourMinuteConverted = 32500;
        }
    };
};

class Prog
{
public:
    void sortPages()
    {
        /*            qsort(customPage, SettingManager::nbCustomPages, sizeof(customPage[0]), sort_desc);
            for (uint8_t iPage = 0; iPage < SettingManager::nbCustomPages; iPage++)
            {
                DEBUGLOGF("[%i] Page[%s][%s][%d]\n", customPage[iPage].id, customPage[iPage].name.c_str(), customPage[iPage].hourMinute.c_str(), customPage[iPage].hourMinuteConverted);
                customPage[iPage].nbElement = 6;
            }*/
    }

    void readData()
    {
        if (SPIFFS.exists(prgFileName))
        {
            DynamicJsonDocument doc(3000);
            File file = SPIFFS.open(prgFileName, FILE_READ);
            DeserializationError error = deserializeJson(doc, file);
            if (!error)
            {
                JsonArray lstProg = doc[F("prog")];
                for (uint8_t i = 0; i < lstProg.size(); i++)
                {
                    m_prgElt[i].fromJson(lstProg[i]);
                }
                //sortPages();
            }
            else
            {
                DEBUGLOGF("Prg file START READING [%s]", error.c_str());
            }
            file.close();
        }
        else
        {
            DEBUGLOG("Prg file does not exist");
        }
    }

    void writeData()
    {
        String ss = toString(false);
        File cfgFile = SPIFFS.open(prgFileName, FILE_WRITE);
        if (cfgFile.print(ss) == ss.length())
        {
            DEBUGLOGF("prog file saved");
        }
        else
        {
            DEBUGLOGF("prog PROBLEM");
        };
        cfgFile.close();
    }

    uint16_t getNext()
    {
        return 0;
    }

    String toString(boolean bJson)
    {
        String ss;
        ss = "\"prog\":[";
        for (uint8_t i = 0; i < maxPrgElt; i++)
        {
            ss += m_prgElt[i].toString(bJson);
            if (i != maxPrgElt - 1)
                ss = ss + ",";
        }
        ss += "]";
        return ss;
    };

private:
    ProgElt m_prgElt[maxPrgElt];
};

#endif