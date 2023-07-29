#include "MessageHandler.h"
#include "TimeCalibration.h"

void MessageHandler::initialize() {
    startupMessage = TimeCalibration::getFormattedString();
    loadMessagesFromFlash();        
    addNewMessage({"I", "I", "I"});
}

void MessageHandler::getEveryMessage(JsonObject& messageObject) {
    messageObject["S"] = startupMessage;

    JsonObject genericMessages = messageObject.createNestedObject("M");

    for (int i = 0; i < NR_OF_MESSAGES; i++) {
        genericMessages[String(i)] = serialized(messages[i].c_str());
    }
}

void MessageHandler::resetUnseenCounter() {
    if (unseenNr > 0) {
        unseenNr = 0;
        serializeMessages();
    }
}

void MessageHandler::addNewMessage(RawMessage rawMessage) {
    for (int i = 0; i < NR_OF_MESSAGES - 1; i++) {
        messages[NR_OF_MESSAGES - 1 - i] = messages[NR_OF_MESSAGES - 1 - i - 1];
    }

    createNewMessage(rawMessage.type, rawMessage.result, rawMessage.additional, messages[0]);

    if (++unseenNr > 10) {
        unseenNr = 10;
    }

    serializeMessages();
}

void MessageHandler::serializeMessages() {
    StaticJsonDocument<1024> doc;

    //doc["C"] = unseenNr;
    doc["S"] = startupMessage;
    JsonObject genericMessages = doc.createNestedObject("M");

    for (int i = 0; i < NR_OF_MESSAGES; i++) {
        genericMessages[String(i)] = serialized(messages[i]);
    }

    String messagesDump;
    serializeJson(doc, messagesDump);
    saveMessagesToFlash(messagesDump);
}

void MessageHandler::createNewMessage(String& type, String& result, String& additional, String& target) {
    target = "";
    StaticJsonDocument<128> newDoc;

    newDoc["T"] = type;
    newDoc["R"] = result;
    newDoc["A"] = additional;
    newDoc["D"] = TimeCalibration::getFormattedString();

    serializeJson(newDoc, target);
}

void MessageHandler::loadMessagesFromFlash() {
    StaticJsonDocument<1024> doc;
    String messagesDump;
    LittleFSHelper::readFile("messages.txt", messagesDump);
    Serial.println("Loaded message dump: " + messagesDump);
    if (messagesDump != "") {
        deserializeJson(doc, messagesDump);
        //unseenNr = doc["C"].as<unsigned int>();
        JsonObject genericMessages =  doc["M"].as<JsonObject>();

        for (int i = 0; i < NR_OF_MESSAGES; i++) {
            messages[i] = genericMessages[String(i)].as<String>();
        }
    }
    else {
        String t = "E";
        String r = "E";
        String a = "E";

        for (int i = 0; i < NR_OF_MESSAGES; i++) {
            createNewMessage(t, r, a, messages[i]);
        }
    }
}

void MessageHandler::saveMessagesToFlash(String& messagesString) {
    //Serial.println("Saving messages:" + messagesString);
    LittleFSHelper::writeFile("messages.txt", messagesString);
}

void MessageHandler::readMessagesFromFlash(String& target) {
    LittleFSHelper::readFile("messages.txt", target);
}
