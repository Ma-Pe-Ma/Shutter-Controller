#include "MessageHandler.h"
#include "TimeCalibration.h"

#include "pb_decode.h"
#include "pb_encode.h"

void MessageHandler::initialize() {
    loadMessagesFromFlash();
    //TODO: add proper value for start
    addNewMessage(Shutter_Event_start, 0);
}

void MessageHandler::addNewMessage(Shutter_Event event, int value) {
    for (int i = 0; i < NR_OF_MESSAGES - 1; i++) {
        messageContainer.genericMessage[NR_OF_MESSAGES - 1 - i] = messageContainer.genericMessage[NR_OF_MESSAGES - 1 - i - 1];
    }

    Shutter_GenericMessage genericMessage = Shutter_GenericMessage_init_default;
    genericMessage.event = event;
    genericMessage.value = value;
    
    genericMessage.datetime = Shutter_Datetime_init_default;
    TimeCalibration::getDatetime(genericMessage.datetime.year, genericMessage.datetime.month, genericMessage.datetime.day, genericMessage.datetime.hour, genericMessage.datetime.minute);

    messageContainer.genericMessage[0] = genericMessage;
}

void MessageHandler::saveMessagesToFlash() {
    uint8_t buffer[300];

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, 300);
    
    if (!pb_encode(&stream, Shutter_MessageContainer_fields, &messageContainer)) {
        Serial.println("Failed to encode");        
    }

    LittleFSHandler::writeFile("messages.txt", buffer, stream.bytes_written);
}

void MessageHandler::loadMessagesFromFlash() {
    String messagesDump;
    LittleFSHandler::readFile("messages.txt", messagesDump);

    if (messagesDump != "") {
        pb_istream_t istream = pb_istream_from_buffer((const unsigned char*) messagesDump.c_str(), messagesDump.length());
        
        if (!pb_decode(&istream, Shutter_MessageContainer_fields, &messageContainer)) {

        }        
    }
    else {
        messageContainer.genericMessage_count = NR_OF_MESSAGES;

        for (int i = 0; i < NR_OF_MESSAGES; i++) {
            messageContainer.genericMessage[i].event = Shutter_Event_empty;
        }
    }

    Shutter_Datetime& startTime = messageContainer.startTime;
    TimeCalibration::getDatetime(startTime.year, startTime.month, startTime.day, startTime.hour, startTime.minute);
}