

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

#include "prog.h"
#include "main.h"

String ProgElt::toString(boolean bJson)
{
    String ss;
    ss += "{\"id\":\"" + String(id) + "\",";
    ss += "\"pgId\":\"" + String(pgId) + "\",";
    ss += "\"pgName\":\"" + pgName + "\",";
    ss += "\"hour\":\"" + hourMinute + "\"}";
    return ss;
}
void ProgElt::fromJson(JsonObject doc)
{
    pgName = doc[F("pgName")].as<String>();
    pgId = doc[F("pgId")];
    setHourMinute(doc[F("hour")].as<String>());
}

void ProgElt::setHourMinute(String hhMm)
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

Prog::Prog()
{
    for (uint8_t i = 0; i < maxPrgElt; i++)
    {
        m_prgElt[i].id = i;
    }
};

void Prog::readData()
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
                m_prgElt[i].id = i;
            }
        }
        else
        {
            DEBUGLOGF("Prg file START READING [%s]", error.c_str());
        }
        file.close();
        sortPages();
    }
    else
    {
        DEBUGLOG("Prg file does not exist");
    }
}

void Prog::writeData()
{
    String ss = "{\"datetime\":{" + wfManager->getHourManager()->toDTString(JSON_TEXT) + "},";
    ss += toString(JSON_TEXT) + "}";

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

uint16_t Prog::getNext(uint16_t hhMn)
{
    int16_t iSelectedPage = -1;
    int16_t iFoundPage = -1;
    for (uint8_t i = 0; i < maxPrgElt; i++)
    {
        DEBUGLOGF("Next page[%s][%s],[%d][%d]\n", m_prgElt[i].pgName.c_str(), m_prgElt[i].hourMinute.c_str(), m_prgElt[i].hourMinuteConverted, hhMn);
        if (iSelectedPage != -1)
        {
            if (m_prgElt[i].hourMinuteConverted < 0 || hhMn < m_prgElt[i].hourMinuteConverted)
            {
                iFoundPage = iSelectedPage;
                break;
            }
        }
        if (hhMn >= m_prgElt[i].hourMinuteConverted)
        {
            iSelectedPage = m_prgElt[i].pgId;
        }
    }
    return iFoundPage;
}

int sort_desc(const void *cmp1, const void *cmp2)
{
    // Need to cast the void * to int *
    ProgElt *a = (ProgElt *)cmp1;
    ProgElt *b = (ProgElt *)cmp2;
    if (a->hourMinuteConverted > b->hourMinuteConverted)
        return 1;
    if (a->hourMinuteConverted == b->hourMinuteConverted)
        return 0;
    if (a->hourMinuteConverted < b->hourMinuteConverted)
        return -1;
}

void Prog::sortPages()
{
    qsort(m_prgElt, maxPrgElt, sizeof(ProgElt), sort_desc);
    for (uint8_t iPage = 0; iPage < maxPrgElt; iPage++)
    {
        DEBUGLOGF("[%i] Page[%i][%s][%s][%d]\n", m_prgElt[iPage].id, m_prgElt[iPage].pgId, m_prgElt[iPage].pgName.c_str(), m_prgElt[iPage].hourMinute.c_str(), m_prgElt[iPage].hourMinuteConverted);
        //        customPage[iPage].nbElement = 6;
    }
}

String Prog::toString(boolean bJson)
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

ProgElt *Prog::getProgElt(uint8_t id)
{
    for (uint8_t i = 0; i < maxPrgElt; i++)
    {
        if (m_prgElt[i].id == id)
            return &m_prgElt[i];
    }
    return NULL;
}
