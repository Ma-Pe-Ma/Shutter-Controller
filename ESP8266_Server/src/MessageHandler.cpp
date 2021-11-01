#include "MessageHandler.h"
#include "TimeCalibration.h"

namespace MessageHandler {    
    unsigned int unseenNr = 0;
    String startupMessage;

    //Whole doc
    StaticJsonDocument<1024> doc;
    String currentMessagesDump;

    //new message
    StaticJsonDocument<128> newDoc;
    String newDump;

    String messages[NR_OF_MESSAGES];

    void Initialize() {
        startupMessage = TimeCalibration::GetFormattedString();
        loadMessagesFromFlash();        
        AddNewMessage("I", "I", "I");
    }

    String GetEveryMessage() {
        return currentMessagesDump;
    }

    void ResetUnseenCounter() {
        if (unseenNr > 0) {
            unseenNr = 0;
            SerializeMessages();
        }
    }

    void AddNewMessage(String type, String result, String additional) {        
        for(int i = 0; i < NR_OF_MESSAGES - 1; i++) {
            messages[NR_OF_MESSAGES - 1 - i] = serialized(messages[NR_OF_MESSAGES - 1 - i - 1]);
        }

        messages[0] = serialized(CreateNewMessage(type, result, additional));

        if (++unseenNr > 10) {
            unseenNr = 10;
        }

        SerializeMessages();
    }

    void SerializeMessages() {
        doc.clear();
        doc["C"] = unseenNr;
        doc["S"] = startupMessage;
        JsonObject genericMessages = doc.createNestedObject("M");

        for(int i = 0; i < NR_OF_MESSAGES; i++) {
            genericMessages[String(i)] = serialized(messages[i]);
        }

        currentMessagesDump = "";
        serializeJson(doc, currentMessagesDump);
        saveMessagesToFlash(currentMessagesDump);
        doc.clear();
    }

    String CreateNewMessage(String type, String result, String additional) {
        newDoc["T"] = type;
        newDoc["R"] = result;
        newDoc["A"] = additional;
        newDoc["D"] = TimeCalibration::GetFormattedString();

        newDump = "";
        serializeJson(newDoc, newDump);
        newDoc.clear();

        return newDump;
    }

    void loadMessagesFromFlash() {
        currentMessagesDump = LittleFSHelper::readFile("messages.txt");
        Serial.println("Loaded message dump: " + currentMessagesDump);
        if (currentMessagesDump != "") {
            deserializeJson(doc, currentMessagesDump);
            unseenNr = doc["C"].as<unsigned int>();
            JsonObject genericMessages =  doc["M"].as<JsonObject>();

            for (int i = 0; i < NR_OF_MESSAGES; i++) {
                messages[i] = genericMessages[String(i)].as<String>();
            }

            doc.clear();
        }
        else {
            for (int i = 0; i < NR_OF_MESSAGES; i++) {
                messages[i] = serialized(CreateNewMessage("E", "E", "E"));
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