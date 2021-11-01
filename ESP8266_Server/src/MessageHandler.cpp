#include "MessageHandler.h"
#include "TimeCalibration.h"

namespace MessageHandler {    
    unsigned int unseenNr = 0;
    String startupMessage;

    JsonObject genericMessages;

    //Whole doc
    StaticJsonDocument<2048> doc;
    //new message
    StaticJsonDocument<128> newDoc;
    String buffer;

    String currentMessagesDump;

    void Initialize() {
        startupMessage = TimeCalibration::GetFormattedString();
        loadMessagesFromFlash();        
        AddNewMessage("I", "I", "I");
    }

    String GetEveryMessage() {      
        return currentMessagesDump;
    }

    void ResetUnseenCounter () {
        if (unseenNr != 0) {
            doc["C"] = 0;
            unseenNr = 0;

            currentMessagesDump = "";
            serializeJson(doc, currentMessagesDump);
            saveMessagesToFlash(currentMessagesDump);
        }        
    }

    void AddNewMessage(String type, String result, String additional) {
        for(int i = 0; i < NR_OF_MESSAGES - 1; i++) {
            genericMessages[String(NR_OF_MESSAGES - 1 - i)] = serialized(genericMessages[String(NR_OF_MESSAGES - 1 - i - 1)].as<String>());
        }

        genericMessages["0"] = serialized(CreateNewMessage(type, result, additional));

        if (++unseenNr > 10) {
            unseenNr = 10;
        }

        doc["C"] = unseenNr;

        currentMessagesDump = "";
        serializeJson(doc, currentMessagesDump);
        saveMessagesToFlash(currentMessagesDump);
    }

    String CreateNewMessage(String type, String result, String additional) {
        newDoc["T"] = type;
        newDoc["R"] = result;
        newDoc["A"] = additional;
        newDoc["D"] = TimeCalibration::GetFormattedString();

        buffer = "";
        serializeJson(newDoc, buffer);
        newDoc.clear();

        return buffer;
    }

    void loadMessagesFromFlash() {
        currentMessagesDump = LittleFSHelper::readFile("messages.txt");
        Serial.println("Loaded message dump: " + currentMessagesDump);
        if (currentMessagesDump != "") {
            deserializeJson(doc, currentMessagesDump);
            unseenNr = doc["C"].as<unsigned int>();
            doc["S"] = startupMessage;
            genericMessages = doc["M"].as<JsonObject>();
        }
        else {
            doc.clear();

            doc["C"] = 0;
            doc["S"] = startupMessage;

            genericMessages = doc.createNestedObject("M");

            for (int i = 0; i < NR_OF_MESSAGES; i++) {
                String messageID = String(i);
                genericMessages[messageID] = serialized(CreateNewMessage("E", "E", "E"));
            }
        }
    }

    void saveMessagesToFlash(String& messagesString) {
        //Serial.println("Savemessages:" + messagesString);
        LittleFSHelper::writeFile("messages.txt", messagesString);
    }

    String readMessagesFromFlash() {
        return LittleFSHelper::readFile("messages.txt");
    }
}