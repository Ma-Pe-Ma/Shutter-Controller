#include "ServerContainer.h"

const char serverCert[] /*PROGMEM*/ = R"EOF(
    paste here content of "cert.txt"
    )EOF";
const char serverKey[] /*PROGMEM*/ =  R"EOF(
    paste here content of "key.txt"
    )EOF";

namespace ServerContainer{

    bool blackout = true;

    BearSSL::ESP8266WebServerSecure secureServer(443);

    void Initialize() {
        
        secureServer.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));

        secureServer.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
        secureServer.on("/DUMP", HTTP_GET, handleDump);
        secureServer.on("/STAT", HTTP_GET, handleStatus);

        secureServer.on("/SCH", HTTP_POST, handleScheduling);
        secureServer.on("/SET", HTTP_POST, handleSetting);
        secureServer.on("/NUL", HTTP_POST, handleZero);
        
        secureServer.onNotFound(handleUnknown); 
    }

    void handleRoot() {
        
    }

    void handleStatus() {
        if (SettingProcess::getCurrentSettingProcess() != nullptr) {
            SendResponse(WAIT);
        }
        else {
            SendResponse(OK);
        }
    }

    void handleSetting() {
        String body = secureServer.arg("body");
        StaticJsonDocument<2048> in;

        DeserializationError err = deserializeJson(in, body);
        if (err == DeserializationError::Ok) {
            JsonObject setting = in.as<JsonObject>();
            float newValue = setting["VALUE"].as<float>();
            SettingProcess::setClientValue(newValue);

            SendResponse(WAIT);
        }
        else {
            String newMessage = CreateNewMessage("SET","JSON", err.c_str());

            MessageHandler::AddNewMessage(newMessage);
            SendResponse(OK);
        }
    }

    void handleScheduling() {
        String body = secureServer.arg("body");
        StaticJsonDocument<2048> doc;

        DeserializationError err = deserializeJson(doc, body);

        if (err == DeserializationError::Ok) {
            JsonObject timingConfig = doc.as<JsonObject>();
            TimingSetter::timingSetter.processMessage(timingConfig);

            SendResponse(WAIT);
        } 
        else {
            String newMessage = CreateNewMessage("SET","JSON", err.c_str());
            MessageHandler::AddNewMessage(newMessage);
            SendResponse(OK);
        }
    }

    void handleZero() {
        String body = secureServer.arg("body");
        StaticJsonDocument<2048> doc;

        DeserializationError err = deserializeJson(doc, body);
        if (err == DeserializationError::Ok) {
            JsonObject zeroObject = doc.as<JsonObject>();
            String zeroState = zeroObject["STATE"];

            if (zeroState == "find") {
                NullProcess::nullProcess.processNull(find);
                SendResponse(WAIT);
            }
            else {
                if (SettingProcess::getCurrentSettingProcess() != nullptr) {
                    NullProcess::nullProcess.processNull(up);
                    String newMessage = CreateNewMessage("NUL","FAIL", "BUSY");
                    MessageHandler::AddNewMessage(newMessage);
                    SendResponse(OK);
                }
                else {
                    if(zeroState == "up") {
                        NullProcess::nullProcess.processNull(up);
                        String newMessage = CreateNewMessage("NUL","OK", "UP");
                        MessageHandler::AddNewMessage(newMessage);
                        SendResponse(OK);
                    }

                    if(zeroState == "down") {
                        NullProcess::nullProcess.processNull(down);
                        String newMessage = CreateNewMessage("NUL","OK", "DOWN");
                        MessageHandler::AddNewMessage(newMessage);
                        SendResponse(OK);
                    }
                }
            }
        }
        else {
            String newMessage = CreateNewMessage("NUL","JSON", err.c_str());
            MessageHandler::AddNewMessage(newMessage);
            SendResponse(OK);
        }
    }

    void handleDump() {
        StaticJsonDocument<2048> doc;

        if (blackout) {
            doc["BO"] = true;
            blackout = false;
        }
        else {
	        doc["BO"] = false;
            JsonObject timingConfig = doc.createNestedObject("TIM");
            Timing::CreateJsonObject(timingConfig);
        }

        doc["MC"] = MessageHandler::unseenNr;
        doc["MES"] = MessageHandler::GetEveryMessage();
        doc["VALUE"] = SettingProcess::getCurrentValue();

        String message;
	    serializeJson(doc, message);

        secureServer.send(200, "text/plain", message);
    }

    void handleUnknown() {
        
    }

    void SendResponse(ResponseType responseType) {
        StaticJsonDocument<2048> out;
        out["MES"] = MessageHandler::GetUnseenMessageDump();

        switch (responseType) {
            case OK:
                out["TYPE"] = "OK";
                break;
            case WAIT:
                out["TYPE"] = "WAIT";
                out["TIME"] = 2;
                break;
        }

        String message;
        serializeJson(out, message);

        secureServer.send(200, "text/plain", message);
    }

    String CreateNewMessage(String type, String result, String additional) {
        StaticJsonDocument<2048> messageDoc;
        messageDoc["TYPE"] = type;
        messageDoc["DATE"] = TimeCalibration::GetFormattedString();
        messageDoc["RESULT"] = result;
        messageDoc["ADD"] = additional;

        String newMessage = "";
        serializeJson(messageDoc, newMessage);

        return newMessage;
    }
}