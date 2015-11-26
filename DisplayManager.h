#pragma once
#include "Middleware.h"

#define BMW_DISPLAY LOW
#define HDMI_DISPLAY HIGH

class DisplayManager : public Middleware
{
public:
   DisplayManager(int displayPin);

   void tick();
   Message process(Message msg);
   void commandHandler(byte* bytes, int length);

   void toggleDisplay();
   void switchToBMWDisplay(unsigned long duration);

private:
   int m_displayPin;
   bool m_forceBMWDisplay;
   unsigned long m_forceDurationOverTimer;

   void setDisplay();
};

DisplayManager::DisplayManager(int displayPin)
{
   m_displayPin = displayPin;
   setDisplay();
}

Message DisplayManager::process(Message msg)
{
   return msg;
}

void DisplayManager::commandHandler(byte* bytes, int length) {}

void DisplayManager::tick()
{
   if (m_forceBMWDisplay && m_forceDurationOverTimer < millis())
   {
      setDisplay();
      m_forceBMWDisplay = false;
   }
}

void DisplayManager::toggleDisplay()
{
   Settings::setBypassDisplay(!Settings::getBypassDisplay());
   setDisplay();
}

void DisplayManager::switchToBMWDisplay(unsigned long duration)
{
   digitalWrite(m_displayPin, BMW_DISPLAY);
   m_forceDurationOverTimer = millis() + duration;
   m_forceBMWDisplay = true;
}

void DisplayManager::setDisplay()
{
   if (Settings::getBypassDisplay())
      digitalWrite(m_displayPin, HDMI_DISPLAY);
   else
      digitalWrite(m_displayPin, BMW_DISPLAY);
}
