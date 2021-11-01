#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "Arduino.h"
#include <ArduinoJson.h>
#include "LittleFSHelper.h"

#define NR_OF_MESSAGES 10

namespace MessageHandler {
    extern String messages[];
    extern unsigned int unseenNr;
    extern String startupMessage;

    void Initialize();
    String GetEveryMessage();
    void AddNewMessage(String, String, String);
    String CreateNewMessage(String, String, String);
    void SerializeMessages();
    void ResetUnseenCounter();

    void saveMessagesToFlash(String&);
    void loadMessagesFromFlash();
    String readMessagesFromFlash();
}

#endif