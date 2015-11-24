#pragma once
class Middleware
{
public:
   virtual void tick();
   virtual Message process(Message msg);
   virtual void commandHandler(byte* bytes, int length);
   // boolean enabled;
   Middleware() {};
   ~Middleware() {};
};

Message Middleware::process(Message msg) {}
