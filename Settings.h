#pragma once

struct UserSettings {
   bool bypassDisplay;
   bool firstBootCompleted;
   int baudRate;
} m_userSettings;

class Settings
{
public:
   const static int pidLength = 8;

   static void init();
   static int getBaudRate();
   static void setBypassDisplay(bool value);
   static bool getBypassDisplay();

private:
   static void save(struct UserSettings *settings);
   static void clear();
   static void firstbootSetup();
};

void Settings::init()
{
   memset(&m_userSettings, 0, sizeof(m_userSettings));
   eeprom_read_block((void*)&m_userSettings, (void*)0, sizeof(m_userSettings));
   if (!m_userSettings.firstBootCompleted)
      Settings::firstbootSetup();
}

void Settings::save(struct UserSettings *settings)
{
   eeprom_write_block((const void*)settings, (void*)0, sizeof(m_userSettings));
}

void Settings::setBypassDisplay(bool value)
{
   m_userSettings.bypassDisplay = value;
   save(&m_userSettings);
}

bool Settings::getBypassDisplay()
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
      false, //bypassDisplay
      true, //firstBootCompleted
      125 //baudRate
   };

   Settings::save(&userSettings);
   Settings::init();
}