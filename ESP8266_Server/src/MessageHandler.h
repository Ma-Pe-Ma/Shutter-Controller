#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "Arduino.h"
#include "LittleFSHandler.h"

#include "../Configuration.h"

#include "Shutter.pb.h"

class MessageHandler {
    Shutter_MessageContainer messageContainer = Shutter_MessageContainer_init_default;

    void saveMessagesToFlash();
    void loadMessagesFromFlash();

public:
    void initialize();
    void addNewMessage(Shutter_Event, int);

    Shutter_MessageContainer getMessageContainer() { return this->messageContainer; }
};

#endif