#pragma once

#include "Global.h"

struct UserSettings {
   byte bypassDisplay;
   byte firstBootDone; // value > 0 means first boot done
   int baudRate;
} m_userSettings;

class Settings
{
public:
   const static int pidLength = 8;

   static void init();
   static int getBaudRate();
   static void setBypassDisplay(byte value);
   static byte getBypassDisplay();

private:
   static void save(struct UserSettings *settings);
   static void clear();
   static void firstbootSetup();
};

void Settings::init()
{
   memset(&m_userSettings, 0, sizeof(m_userSettings));
   eeprom_read_block((void*)&m_userSettings, (void*)0, sizeof(m_userSettings));
   if (m_userSettings.firstBootDone == 0)
      Settings::firstbootSetup();
}

void Settings::save(struct UserSettings *settings)
{
#ifdef DEBUG
   Serial.println("Settings");
   Serial.println("--------");
   Serial.print("bypassDisplay: ");
   Serial.println(m_userSettings.bypassDisplay);
   Serial.print("firstBootDone: ");
   Serial.println(m_userSettings.firstBootDone);
   Serial.print("baudRate: ");
   Serial.println(m_userSettings.baudRate);
#endif

   eeprom_write_block((const void*)settings, (void*)0, sizeof(m_userSettings));
}

void Settings::setBypassDisplay(byte value)
{
   m_userSettings.bypassDisplay = value;
   save(&m_userSettings);
}

byte Settings::getBypassDisplay()
{
   return m_userSettings.bypassDisplay;
}

int Settings::getBaudRate()
{
   return m_userSettings.baudRate;
}

void Settings::clear()
{
   for (int i = 0; i < 512; i++)
      EEPROM.write(i, 0);
}

void Settings::firstbootSetup()
{
   Settings::clear();

   struct UserSettings userSettings = {
      0, //bypassDisplay
      1, //firstBootDone
      500 //baudRate
   };

   Settings::save(&userSettings);
   Settings::init();
}
