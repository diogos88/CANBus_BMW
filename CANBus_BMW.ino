#include <avr/wdt.h>
#include <EEPROM.h>
#include <CANBus.h>
#include <Message.h>
#include <QueueArray.h>

#include "Global.h"
#include "Settings.h"
#include "DisplayManager.h"
#include "BMWWheelButton.h"

#define BOOT_LED 13
#define DISPLAY_PIN 8

QueueArray<Message> m_readQueue;
QueueArray<Message> m_writeQueue;

CANBus m_CANBus(9 /*CAN_SELECT*/, 4/*CAN_RESET*/, 1, "Bus 1");

DisplayManager *m_display = new DisplayManager(DISPLAY_PIN);

Middleware *activeMiddleware[] = {
   new BMWWheelButton(m_display)
};
int activeMiddlewareLength = (int)(sizeof(activeMiddleware) / sizeof(activeMiddleware[0]));

void setup()
{
   Serial.begin(115200); // USB

   Settings::init();
   delay(1);

   Serial.print("Baud rate = ");
   Serial.println(m_userSettings.baudRate);

   pinMode(BOOT_LED, OUTPUT);

   pinMode(3 /*CAN1INT_D*/, INPUT);
   pinMode(4 /*CAN1RESET*/, OUTPUT);
   digitalWrite(4 /*CAN1RESET*/, LOW);

   // Init CAN Bus
   m_CANBus.begin();
   m_CANBus.setClkPre(1);
   m_CANBus.baudConfig(m_userSettings.baudRate);
   m_CANBus.setRxInt(true);
   m_CANBus.bitModify(RXB0CTRL, 0x04, 0x04); // Set buffer rollover enabled
   m_CANBus.disableFilters();
   m_CANBus.setMode(NORMAL);

   // Blink LED boot finished
   for (int l = 0; l < 5; l++)
   {
      digitalWrite(BOOT_LED, HIGH);
      delay(50);
      digitalWrite(BOOT_LED, LOW);
      delay(50);
   }
}

void loop()
{
   for (int i = 0; i <= activeMiddlewareLength - 1; i++)
      activeMiddleware[i]->tick();

   //    CAN Bus read
   if (digitalRead(3/*CAN1INT_D*/) == 0)
      readBus(m_CANBus);

#ifdef DEBUG
   Serial.print(millis());
   Serial.print(" - Number of messages in read queue : ");
   Serial.println(m_readQueue.count());
#endif

   // Process CAN messages
   if (!m_readQueue.isEmpty())
   {
      Message msg = m_readQueue.pop();
      for (int i = 0; i <= activeMiddlewareLength - 1; i++)
         msg = activeMiddleware[i]->process(msg);

      if (msg.dispatch && !m_writeQueue.isFull())
         m_writeQueue.push(msg);
   }

#ifdef DEBUG
   Serial.print(millis());
   Serial.print(" - Number of messages in write queue : ");
   Serial.println(m_writeQueue.count());
#endif

   // Release CAN messages
   boolean error = false;
   while (!m_writeQueue.isEmpty() && !error)
   {
      Message msg = m_writeQueue.pop();
      error = !sendMessage(msg, m_CANBus);

      if (error)
         m_writeQueue.push(msg); // When TX Failure, add back to queue
   }
}

boolean sendMessage(Message msg, CANBus bus)
{
   if (!msg.dispatch)
      return true;

   int ch = bus.getNextTxBuffer();
   if (ch < 0 || ch > 2)
      return false; // All TX buffers full

   digitalWrite(BOOT_LED, HIGH);
   bus.loadFullFrame(ch, msg.length, msg.frame_id, msg.frame_data);
   bus.transmitBuffer(ch);
   digitalWrite(BOOT_LED, LOW);

   return true;
}

void readBus(CANBus bus)
{
   byte rx_status = bus.readStatus();
   if (rx_status & 0x1)
      readMsgFromBuffer(bus, 0, rx_status);

   if (rx_status & 0x2)
      readMsgFromBuffer(bus, 1, rx_status);
}

void readMsgFromBuffer(CANBus bus, byte bufferId, byte rx_status)
{
   // Abort if readQueue is full
   if (m_readQueue.isFull())
      return;

   Message msg;
   msg.busStatus = rx_status;
   msg.busId = bus.busId;
   bus.readFullFrame(bufferId, &msg.length, msg.frame_data, &msg.frame_id);
   m_readQueue.push(msg);
}
