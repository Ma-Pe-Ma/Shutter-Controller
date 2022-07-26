#include "ServerContainer.h"
#include "../Configuration.h"
#include "../Keys.h"
#include "BrowserClient.h"
#include "Hash.h"

namespace ServerContainer {
    BearSSL::ESP8266WebServerSecure secureServer(HTTPS_PORT);
    ESP8266WebServer server(HTTP_PORT);
    String secureAddress = "https://" + String(DDNS_DOMAIN);

    void initialize() {
        secureServer.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));

        secureServer.on("/", HTTP_GET, handleRoot);
        secureServer.on("/S", HTTP_GET, handleStatus);
        secureServer.on("/D", HTTP_GET, handleGetDump); 

        secureServer.on("/V", HTTP_POST, handlePostSetting);
        secureServer.on("/T", HTTP_POST, handlePostTiming);  
        secureServer.on("/Z", HTTP_POST, handlePostZero);
        
        secureServer.on("/L", HTTP_POST, handleLogin);
        secureServer.on("/O", HTTP_GET, handleLogout);
        
        secureServer.onNotFound(handleRedirectSecure); 
        
        secureServer.collectHeaders("Cookie");
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
        if (secureServer.hasHeader("Cookie") && isCookieValid()) {
            secureServer.send(200, "text/html", controlPage);
        }
        else {
            secureServer.send(200, "text/html", loginPage);
            Serial.println("Showing login page!");
        }
    }

    void handleStatus() {
        DynamicJsonDocument docOut(1024);

        if (!authenticationCheck()) {
            handleRedirectSecure();
            return;
        }

        SettingProcess* current = SettingProcess::getCurrentSettingProcess();
        if (current != nullptr) {
            createGenericResponse(current->getRemainingTime(), docOut);
        }
        else {
            createGenericResponse(0, docOut);
        }

        String response;
        serializeJson(docOut, response);

        secureServer.send(200, "text/plain", response);
        response = "";

        //if (current == nullptr) {
        //    MessageHandler::ResetUnseenCounter();
        //}        

        Serial.println("Status was sent!");
    }

    void handleGetDump() {
        DynamicJsonDocument docOut(2048);

        if (!authenticationCheck()) {
            handleRedirectSecure();
            return;
        }

        String timingsString;
        Timing::readTimingsFromFlash(timingsString);
        docOut["T"] = serialized(timingsString.c_str());
        
        SettingProcess* current = SettingProcess::getCurrentSettingProcess();
        if (current != nullptr) {
            createGenericResponse(current->getRemainingTime(), docOut);
        }
        else {
            createGenericResponse(0, docOut);
        }
    
        String response;
	    serializeJson(docOut, response);

        //if (current == nullptr) {
           // MessageHandler::ResetUnseenCounter();
        //}

        secureServer.send(200, "text/plain", response);
        Serial.println("Dump sent back!");
    }

    void handlePostSetting() {
        DynamicJsonDocument docOut(1024);
        StaticJsonDocument<512> docIn;

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
            MessageHandler::addNewMessage("J","S", err.c_str()); 
            wait = 0;
        }

        createGenericResponse(wait, docOut);

        String response;
        serializeJson(docOut, response);

        secureServer.send(200, "text/plain", response);
        response = "";

        Serial.println("New value was posted: " + String(intValue));
    }

    void handlePostTiming() {
        DynamicJsonDocument docOut(1024);
        StaticJsonDocument<768> docIn;

        if (!authenticationCheck()) {
            handleRedirectSecure();
            return;
        }

        DeserializationError err = deserializeJson(docIn, secureServer.arg("plain"));

        if (err == DeserializationError::Ok) {
            JsonObject timingConfig = docIn.as<JsonObject>();
            Timing::parseTimings(timingConfig);
            Timing::disableEarlierSettings();
            MessageHandler::addNewMessage("S","T", "O");
        } 
        else {
            MessageHandler::addNewMessage("J","T", err.c_str());
        }
        
        docIn.clear();
        yield();

        createGenericResponse(0, docOut);

        String response;
        serializeJson(docOut, response);
        docOut.clear();

        secureServer.send(200, "text/plain", response);
        response = "";

        yield();

        Timing::saveTimingsToFlash(secureServer.arg("plain"));
        Serial.println("Timings were updated!");
    }

    void handlePostZero() {
        DynamicJsonDocument docOut(1024);
        StaticJsonDocument<512> docIn;

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
                    MessageHandler::addNewMessage("Z","F", "B");
                    retryTime = 3;
                }
                else {
                    if(zeroState == "up") {
                        ZeroProcess::zeroProcess.processNull(up);
                        MessageHandler::addNewMessage("Z","O", "U");
                        retryTime = 0;
                    }

                    if(zeroState == "down") {
                        ZeroProcess::zeroProcess.processNull(down);
                        MessageHandler::addNewMessage("Z","O", "D");
                        retryTime = 0;
                    }
                }
            }
        }
        else {
            MessageHandler::addNewMessage("J","Z", err.c_str());
            retryTime = 0;
        }

        createGenericResponse(retryTime, docOut);
        String response;
        serializeJson(docOut, response);
        secureServer.send(200, "text/plain", response);

        Serial.println("Zeroing was set: " + zeroState);
    }

    void createGenericResponse(int waitTime, JsonDocument& document) {
        JsonObject genericResponse = document.createNestedObject("G");
        
        if (waitTime > 0) {
            genericResponse["R"] = waitTime;
            return;
        }

        JsonObject messageObject = genericResponse.createNestedObject("M");
        MessageHandler::getEveryMessage(messageObject);

        genericResponse["V"] = (int) (SettingProcess::getCurrentValue() * 100);
        genericResponse["R"] = waitTime;
    }

    bool authenticationCheck() {
        if (isCookieValid()) {
            return true;
        }

        if (secureServer.args() < 2) {
            return false;
        }

        if (secureServer.hasArg("username") && secureServer.hasArg("password")) {
            if (secureServer.arg("username") == USER_NAME && secureServer.arg("password") == PASSWORD) {
                return true;
            }
        }

        return false;
    }

    unsigned long lastCookieUsage = 0;
    String currentCookie;
    unsigned long cookieStartTime = 0;

    bool isCookieValid() {
        String receivedCookie = secureServer.header("Cookie");

        //Sign out after 5 minutes of idle state
        if (millis() - lastCookieUsage >  5 * 60 * 1000) {
            return false;
        }

        //Force sign out 30 minutes after login
        if (millis() - cookieStartTime > 20 * 60 * 1000) {
            return false;
        } 

        if (currentCookie.indexOf(receivedCookie) != -1) {
            lastCookieUsage = millis();
            return true;
        }

        return false;
    }

    String generateCookie() {
        lastCookieUsage = millis();
        currentCookie = "session=" + sha1(USER_NAME + String(lastCookieUsage));
        cookieStartTime = millis();

        return currentCookie;
    }

    void handleLogin() {
        if (secureServer.hasHeader("Cookie") && isCookieValid()) {
            Serial.println("ALREADY SIGNED IN!");
            handleRoot();
        }
        else {
            if (authenticationCheck()) {
                Serial.println("Signing IN...");
                secureServer.sendHeader("Cache-Control", "no-cache");
                secureServer.sendHeader("Set-Cookie", generateCookie());
                secureServer.sendHeader("Location", "/");
                secureServer.send(302, "text/html", "");
            }
            else {
                secureServer.send(200, "text/html", loginFail);
            }    
        }
    }

    void handleLogout() {
        Serial.println("Signing out...");
        secureServer.sendHeader("Set-Cookie", "session=null");
        secureServer.sendHeader("Location", "/");
        secureServer.send(302, "text/html", "");
    }
}
