

#include "BaseSettingManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

BaseSettingManager::BaseSettingManager(unsigned char pinLed) : BaseManager(pinLed){
  m_iEEprom = 0;
  EEPROM.begin(512);
}

String BaseSettingManager:: toString(boolean bJson = false){
  return "VER["+ String(m_version) + "] SSID[" + String(m_ssid) + "] PASS[" + m_password + "] privateKey[" + m_privateKey + "] publicKey[" + m_publicKey + "]";
}

uint8_t BaseSettingManager::readData(){
  switchOn();
  m_iEEprom = 0;
  m_version = readEEPROM();
  if (m_version != EEPROM_VERSION) {
    m_iEEprom = 0;
    //m_version = EEPROM_VERSION;
    setStatus(millis(), m_iEEprom, "dif ver");
    Serial.printf("Version dif [%d,%d]/d",m_version,EEPROM_VERSION);
  } else {
    setStatus(millis(), m_iEEprom, "Ok");
  }
  readEEPROM(m_ssid,32);
  readEEPROM(m_password,64);
  readEEPROM(m_privateKey,64);
  readEEPROM(m_publicKey,64);
  //setStatus(millis(), m_iEEprom, "read");
  switchOff();
  return m_iEEprom;
}
uint8_t BaseSettingManager::writeData(){
  switchOn();
  m_iEEprom=0;
  writeEEPROM(EEPROM_VERSION);
  writeEEPROM(m_ssid);
  writeEEPROM(m_password);
  writeEEPROM(m_privateKey);
  writeEEPROM(m_publicKey);
  EEPROM.commit();
  setStatus( m_iEEprom, "written");
  switchOff();
  return m_iEEprom;
}

uint8_t BaseSettingManager::clearData(){
  switchOn();
  for (int i = 0; i < 512; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  switchOff();
  return m_iEEprom;
}

/* uint8_t BaseSettingManager::writeEEPROM(int16_t value){
  char tmp[5];
  itoa(value,tmp,16);
  /*Serial.print(m_iEEprom);Serial.print(":");Serial.print(value);
  Serial.print(" , "); Serial.println(tmp);
  return writeEEPROM(tmp);
}*/

uint8_t BaseSettingManager::writeEEPROM(int32_t value){
  char tmp[10];
  itoa(value,tmp,32);
  /*Serial.print(m_iEEprom);Serial.print(":");Serial.print(value);
  Serial.print(" , "); Serial.println(tmp);*/
  return writeEEPROM(tmp);
}

uint8_t BaseSettingManager::writeEEPROM(char *str){
  for (int iString = 0; str[iString] != 0; iString++,m_iEEprom++ )  {
    EEPROM.write(m_iEEprom, str[iString]);
    //delay(5);
    //Serial.print(m_iEEprom);Serial.print(":");DEBUGLOG(str[iString]);
  }
  //delay(5);
  EEPROM.write(m_iEEprom,0);
  //Serial.print(m_iEEprom);DEBUGLOG(":--");
  m_iEEprom++;
  return 0;
}

/* int16_t BaseSettingManager::readEEPROM(){
  char tmp[8];
  readEEPROM(tmp,5);
  /*Serial.print(m_iEEprom);Serial.print(":");Serial.print(tmp);
  Serial.print(" , "); Serial.println(strtol(tmp, NULL,16));
  return strtol(tmp, NULL,16);
}*/

int32_t BaseSettingManager::readEEPROM(){
  char tmp[20];
  readEEPROM(tmp,20);
  /*Serial.print(m_iEEprom);Serial.print(":");Serial.print(tmp);
  Serial.print(" , "); Serial.println(strtol(tmp, NULL,16));*/
  return strtol(tmp, NULL,32);
}

char *BaseSettingManager::readEEPROM(char *str,uint8_t maxChar){
  int iString = 0;
  //Serial.print("Read  ============= ");Serial.println(m_iEEprom);
  while (iString < maxChar) {
    str[iString] = EEPROM.read(m_iEEprom);
    //Serial.print(m_iEEprom);Serial.print(":");
    m_iEEprom++;
    if (str[iString] == 0) {
    //  DEBUGLOG("--");
      break;
    }/* else {
      DEBUGLOG(str[iString]);
    }*/
    iString++;
    //delay(50);
  }
  return str;

}
