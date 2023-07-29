#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "Arduino.h"
#include <ArduinoJson.h>
#include "LittleFSHelper.h"
#include "RawMessage.h"

#include "../Configuration.h"

class MessageHandler {
    String messages[NR_OF_MESSAGES];
    unsigned int unseenNr = 0;
    String startupMessage;
public:
    void initialize();
    void getEveryMessage(JsonObject&);
    void addNewMessage(RawMessage);
    void createNewMessage(String&, String&, String&, String&);
    void serializeMessages();
    void resetUnseenCounter();

    void saveMessagesToFlash(String&);
    void loadMessagesFromFlash();
    void readMessagesFromFlash(String&);
};

#endif