#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "Arduino.h"
#include <ArduinoJson.h>
#include "LittleFSHelper.h"

namespace MessageHandler {
    extern String messages[];
    extern unsigned int unseenNr;
    extern String startupMessage;

    void initialize();
    void getEveryMessage(JsonObject&);
    void addNewMessage(String, String, String);
    void createNewMessage(String&, String&, String&, String&);
    void serializeMessages();
    void resetUnseenCounter();

    void saveMessagesToFlash(String&);
    void loadMessagesFromFlash();
    void readMessagesFromFlash(String&);
}

#endif