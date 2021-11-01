#include "ServerContainer.h"
#include "../Configuration.h"

namespace ServerContainer {
    BearSSL::ESP8266WebServerSecure secureServer(HTTPS_PORT);
    ESP8266WebServer server(HTTP_PORT);
    String secureAddress = "https://" + String(DDNS_DOMAIN);

    StaticJsonDocument<2048> docOut;
    StaticJsonDocument<768> docIn;
    String response;

    void initialize() {
        secureServer.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
        //Serial.print(serverCert);
        //Serial.println();

        secureServer.on("/", HTTP_GET, handleRoot);
        secureServer.on("/S", HTTP_GET, handleStatus);
        secureServer.on("/D", HTTP_GET, handleGetDump); 

        secureServer.on("/V", HTTP_POST, handlePostSetting);
        secureServer.on("/T", HTTP_POST, handlePostTiming);  
        secureServer.on("/Z", HTTP_POST, handlePostZero);
        
        secureServer.onNotFound(handleRedirectSecure); 

        secureServer.begin();

        server.on("/", HTTP_GET, handleRedirect);
        server.onNotFound(handleRedirect);
        server.begin();
    }

    void handleRedirect() {
        server.sendHeader("Location", secureAddress, true);
        server.send(302, "text/plain", "");
        Serial.println("Redirect to secure!");
    }

    void handleRedirectSecure() {
        secureServer.sendHeader("Location", String("/"), true);
        secureServer.send(302, "text/plain", "");
        Serial.println("Redirect to root secure!");
    }

    void handleRoot() {
        secureServer.send(200, "text/html", DEFAULT_PAGE);
        Serial.println("Root was sent!");
    }

    void handleStatus() {
        docOut.clear();

        if (!authenticationCheck()) {
            handleRedirectSecure();
            return;
        }

        if (SettingProcess::getCurrentSettingProcess() != nullptr) {
            createGenericResponse(3);
        }
        else {
            createGenericResponse(0);
        }

        serializeJson(docOut, response);
        docOut.clear();

        secureServer.send(200, "text/plain", response);
        response = "";

        Serial.println("Status was sent!");
    }

    void handleGetDump() {
        docOut.clear();

        if (!authenticationCheck()) {
            handleRedirectSecure();
            return;
        }

        docOut["T"] = serialized(Timing::readTimingsFromFlash());
        
        if (SettingProcess::getCurrentSettingProcess() != nullptr) {
            createGenericResponse(3, true);
        }
        else {
            createGenericResponse(0, true);
        }
    
	    serializeJson(docOut, response);
        docOut.clear();

        secureServer.send(200, "text/plain", response);
        response = "";
        Serial.println("Dump sent back!");
    }

    void handlePostSetting() {
        docOut.clear();
        docIn.clear();

        if (!authenticationCheck()) {
            handleRedirectSecure();
            return;
        }

        DeserializationError err = deserializeJson(docIn, secureServer.arg("plain"));

        int wait = 0;
        int intValue = -1;

        if (err == DeserializationError::Ok) {
            JsonObject setting = docIn.as<JsonObject>();
            intValue = setting["V"].as<int>();
            float newValue = 1.0f * intValue / 100;
            SettingProcess::setClientValue(newValue);
            wait = 3;
        }
        else {
            MessageHandler::AddNewMessage("J","S", err.c_str()); 
            wait = 0;
        }

        docIn.clear();

        createGenericResponse(wait);

        serializeJson(docOut, response);
        docOut.clear();

        secureServer.send(200, "text/plain", response);
        response = "";

        Serial.println("New value was posted: " + String(intValue));
    }

    void handlePostTiming() {
        docOut.clear();
        docIn.clear();

        if (!authenticationCheck()) {
            handleRedirectSecure();
            return;
        }

        String body = secureServer.arg("plain");

        DeserializationError err = deserializeJson(docIn, body);

        for (int i = 0; i < secureServer.args(); i++) {
            Serial.println(String(i) + ". arg: " + secureServer.argName(i) + " - " + secureServer.arg(i));
        }

        if (err == DeserializationError::Ok) {
            JsonObject timingConfig = docIn.as<JsonObject>();
            Timing::parseTimings(timingConfig);
            Timing::disableEarlierSettings();
            MessageHandler::AddNewMessage("S","T", "O");
        } 
        else {
            MessageHandler::AddNewMessage("J","T", err.c_str());
        }
        docIn.clear();

        createGenericResponse(0);

        serializeJson(docOut, response);
        docOut.clear();

        secureServer.send(200, "text/plain", response);
        response = "";

        Timing::saveTimingsToFlash(body);
        Serial.println("Timings were updated!");
    }

    void handlePostZero() {
        docOut.clear();
        docIn.clear();

        if (!authenticationCheck()) {
            handleRedirectSecure();
            return;
        }

        int8_t retryTime = 0;

        String zeroState = "";

        DeserializationError err = deserializeJson(docIn, secureServer.arg("plain"));
        if (err == DeserializationError::Ok) {
            JsonObject zeroObject = docIn.as<JsonObject>();
            zeroState = zeroObject["Z"].as<String>();

            if (zeroState == "find") {
                ZeroProcess::zeroProcess.processNull(find);
                retryTime = 3;
            }
            else {
                if (SettingProcess::getCurrentSettingProcess() != nullptr) {
                    MessageHandler::AddNewMessage("Z","F", "B");
                    retryTime = 3;
                }
                else {
                    if(zeroState == "up") {
                        ZeroProcess::zeroProcess.processNull(up);
                        MessageHandler::AddNewMessage("Z","O", "U");
                        retryTime = 0;
                    }

                    if(zeroState == "down") {
                        ZeroProcess::zeroProcess.processNull(down);
                        MessageHandler::AddNewMessage("Z","O", "D");
                        retryTime = 0;
                    }
                }
            }
        }
        else {
            MessageHandler::AddNewMessage("J","Z", err.c_str());
            retryTime = 0;
        }

        docIn.clear();

        createGenericResponse(retryTime);

        serializeJson(docOut, response);
        docOut.clear();

        secureServer.send(200, "text/plain", response);
        response = "";

        Serial.println("Zeroing was set: " + zeroState);
    }

    void createGenericResponse(int waitTime, bool root) {
        JsonObject genericResponse;
        
        if (root) {
            genericResponse = docOut.createNestedObject("G");
        }
        else {
            genericResponse = docOut.to<JsonObject>();
        }
        
        if (waitTime > 0) {
            genericResponse["T"] = waitTime;
            return;
        }
      
        genericResponse["M"] = serialized(MessageHandler::GetEveryMessage());
        genericResponse["V"] = (int) (SettingProcess::getCurrentValue() * 100);
        genericResponse["T"] = waitTime;

        MessageHandler::ResetUnseenCounter();
    }

    bool authenticationCheck() {
        if (secureServer.args() < 2) {
            return false;
        }

        bool usernameCheck = false;
        if (secureServer.argName(0) == "username" && secureServer.arg(0) == USER_NAME) {
            usernameCheck = true;
        }

        bool passwordCheck = false;
        if (secureServer.argName(1) == "password" && secureServer.arg(1) == PASSWORD) {
            passwordCheck = true;
        }

        return usernameCheck && passwordCheck;
    }
}