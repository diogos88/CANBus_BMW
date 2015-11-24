#pragma once
#include "Middleware.h"
#include "DisplayManager.h"

class BMWWheelButton : public Middleware
{
public:
   BMWWheelButton(DisplayManager *display);

   void tick();
   Message process(Message msg);
   void commandHandler(byte* bytes, int length);

private:
   DisplayManager *m_display;
   void toggleDisplay();

   bool m_btn001Pressed;
   bool m_btn002Pressed;
   unsigned int m_toggleDisplayEnabled;
};

BMWWheelButton::BMWWheelButton(DisplayManager *display)
{
   m_display = display;

   m_btn001Pressed = false;
   m_btn002Pressed = false;
   m_toggleDisplayEnabled = true;
}

Message BMWWheelButton::process(Message msg)
{
   // TODO
   //    Process can message

   if (m_btn001Pressed && m_btn002Pressed)
   {
      m_display->toggleDisplay();
      m_toggleDisplayEnabled = false;
   }

   if (!m_btn001Pressed && !m_btn002Pressed)
   {
      m_toggleDisplayEnabled = true;
   }

   return msg;
}

void BMWWheelButton::tick() {}

void BMWWheelButton::commandHandler(byte* bytes, int length) {}
