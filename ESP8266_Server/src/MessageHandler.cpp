#include "MessageHandler.h"
#include "TimeCalibration.h"
#include "../Configuration.h"

namespace MessageHandler {    
    unsigned int unseenNr = 0;
    String startupMessage;
    String messages[NR_OF_MESSAGES];

    void initialize() {
        startupMessage = TimeCalibration::getFormattedString();
        loadMessagesFromFlash();        
        addNewMessage("I", "I", "I");
    }

    void getEveryMessage(JsonObject& messageObject) {
        messageObject["S"] = startupMessage;

        JsonObject genericMessages = messageObject.createNestedObject("M");

        for(int i = 0; i < NR_OF_MESSAGES; i++) {
            genericMessages[String(i)] = serialized(messages[i].c_str());
        }
    }

    void resetUnseenCounter() {
        if (unseenNr > 0) {
            unseenNr = 0;
            serializeMessages();
        }
    }

    void addNewMessage(String type, String result, String additional) {        
        for(int i = 0; i < NR_OF_MESSAGES - 1; i++) {
            messages[NR_OF_MESSAGES - 1 - i] = messages[NR_OF_MESSAGES - 1 - i - 1];
        }

        createNewMessage(type, result, additional, messages[0]);

        if (++unseenNr > 10) {
            unseenNr = 10;
        }

        serializeMessages();
    }

    void serializeMessages() {
        StaticJsonDocument<1024> doc;

        //doc["C"] = unseenNr;
        doc["S"] = startupMessage;
        JsonObject genericMessages = doc.createNestedObject("M");

        for(int i = 0; i < NR_OF_MESSAGES; i++) {
            genericMessages[String(i)] = serialized(messages[i]);
        }

        String messagesDump;
        serializeJson(doc, messagesDump);
        saveMessagesToFlash(messagesDump);
    }

    void createNewMessage(String& type, String& result, String& additional, String& target) {
        target = "";
        StaticJsonDocument<128> newDoc;

        newDoc["T"] = type;
        newDoc["R"] = result;
        newDoc["A"] = additional;
        newDoc["D"] = TimeCalibration::getFormattedString();
 
        serializeJson(newDoc, target);
    }

    void loadMessagesFromFlash() {
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

    void saveMessagesToFlash(String& messagesString) {
        //Serial.println("Saving messages:" + messagesString);
        LittleFSHelper::writeFile("messages.txt", messagesString);
    }

    void readMessagesFromFlash(String& target) {
        LittleFSHelper::readFile("messages.txt", target);
    }
}