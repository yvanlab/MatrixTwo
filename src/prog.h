
#ifndef prog_h
#define prog_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"
#include "ArduinoJson.h"

#else

#include "WProgram.h"

#endif

#define maxPrgElt 10
const char prgFileName[] = "/prog.json";

class ProgElt
{
public:
    int8_t id = -1;
    int16_t pgId = -1;
    String hourMinute;
    int16_t hourMinuteConverted = 32500;
    String pgName;
    int16_t duration = 10; //s
    String toString(boolean bJson);
    void fromJson(JsonObject doc);
    void setHourMinute(String hhMm);
};

class Prog
{
public:
    Prog();
    void sortPages();
    void readData();
    void writeData();
    uint16_t getNext(uint16_t hhMn);
    String toString(boolean bJson);
    ProgElt *getProgElt(uint8_t id);

private:
    ProgElt m_prgElt[maxPrgElt];
};

#endif