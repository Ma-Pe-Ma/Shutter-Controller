#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "Arduino.h"
#include <ArduinoJson.h>

#define NR_OF_MESSAGES 10

namespace MessageHandler {
    extern String messages[];
    extern int unseenNr;

    void Initialize();
    void AddNewMessage(String);
    String* GetUnseenMessage();
    String GetUnseenMessageDump();
    String GetEveryMessage();
}

#endif