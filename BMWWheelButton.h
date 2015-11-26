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
};

BMWWheelButton::BMWWheelButton(DisplayManager *display)
{
   m_display = display;

   m_btn001Pressed = false;
   m_btn002Pressed = false;
}

Message BMWWheelButton::process(Message msg)
{
   bool statusChanged = false;

   // TODO
   /*
   Process can message
   -------------------
   if can.message == btn001.code
      bool pressed = can.message.value
      if m_btn001Pressed != pressed
         m_btn001Pressed = pressed
         statusChanged = true

    if can.message == btn002.code
       bool pressed = can.message.value
       if m_btn002Pressed != pressed
         m_btn002Pressed = pressed
         statusChanged = true
   */

   if (statusChanged)
   {
#ifdef DEBUG
      Serial.println("Button status changed");
      Serial.print("Button 1 : ");
      Serial.println((m_btn001Pressed == 0) ? "false" : "true");
      Serial.print("Button 2 : ");
      Serial.println((m_btn002Pressed == 0) ? "false" : "true");
#endif

      if (m_btn001Pressed && m_btn002Pressed)
         m_display->toggleDisplay();
   }

   return msg;
}

void BMWWheelButton::tick() {}

void BMWWheelButton::commandHandler(byte* bytes, int length) {}
