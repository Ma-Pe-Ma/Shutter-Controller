#include "MessageHandler.h"

namespace MessageHandler {
    String messages[NR_OF_MESSAGES];
    int unseenNr = 0;

    void Initialize() {
        for (int i = 0; i < NR_OF_MESSAGES; i++) {
            messages[i] = "";
        }
    }

    void AddNewMessage(String newMessage) {
        for(int i = 0; i < NR_OF_MESSAGES - 1; i++) {
            messages[NR_OF_MESSAGES - 1 - i] = messages[NR_OF_MESSAGES - 1 - i - 1];
        }

        messages[0] = newMessage;

        if (++unseenNr>10) {
            unseenNr = 10;
        }
    }

    String* GetUnseenMessage() {
        if (unseenNr == 0) {
            return nullptr;
        }

        return &messages[--unseenNr];
    }

    String GetUnseenMessageDump() {
        String messages = "";

        int i = 0;

        StaticJsonDocument<2048> doc;
        String* unseenMessage;

        while ((unseenMessage = GetUnseenMessage()) != nullptr) {
            i++;
            String messageID = "" + i;
            doc[messageID] = *unseenMessage;
        }

        if (i == 0) {
            return "";
        }

        serializeJson(doc, messages);
        return messages;
    }

    String GetEveryMessage() {
        unseenNr = 0;

        StaticJsonDocument<2048> doc;

        for (int i = 0; i < NR_OF_MESSAGES; i++) {
            String messageID = "" + i;
            doc[messageID] = messages[i];
        }

        String output;
        serializeJson(doc, output);
        return output;
    }
}