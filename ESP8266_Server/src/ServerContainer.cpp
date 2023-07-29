#include "ServerContainer.h"

#include "../Keys.h"
#include "BrowserClient.h"
#include "Hash.h"

void ServerContainer::initialize() {
    messageHandler.initialize();
    processQueue.initialize();
    timingContainer.initialize(&processQueue);

    processQueue.setMessageProcessor([this](RawMessage rawMessage) -> void {
        this->messageHandler.addNewMessage(rawMessage);
    });

    secureServer.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));

    //handle login page
    secureServer.on("/", HTTP_GET, [this]() -> void {
        if (this->secureServer.hasHeader("Cookie") && this->isCookieValid()) {
            this->secureServer.send(200, "text/html", controlPage);
        }
        else {
            this->secureServer.send(200, "text/html", loginPage);
            Serial.println("Showing login page!");
        }
    });

    //handle getting the current value, message history
    secureServer.on("/S", HTTP_GET, [this]() -> void {
        DynamicJsonDocument docOut(1024);

        if (!this->authenticationCheck()) {
            this->handleRedirectSecure();
            return;
        }

        SettingProcess* current = this->processQueue.getCurrentSettingProcess();
        if (current != nullptr) {
            this->createGenericResponse(current->getRemainingTime(), docOut);
        }
        else {
            this->createGenericResponse(0, docOut);
        }

        String response;
        serializeJson(docOut, response);

        this->secureServer.send(200, "text/plain", response);
        response = "";

        //if (current == nullptr) {
        //    messageHandler.ResetUnseenCounter();
        //}        

        Serial.println("Status was sent!");
    });

    // handle getting the dump: current value, message history and the timings
    secureServer.on("/D", HTTP_GET, [this]() -> void {
        DynamicJsonDocument docOut(2048);

        if (!this->authenticationCheck()) {
            this->handleRedirectSecure();
            return;
        }

        String timingsString;
        this->timingContainer.readTimingsFromFlash(timingsString);
        docOut["T"] = serialized(timingsString.c_str());
        
        SettingProcess* current = processQueue.getCurrentSettingProcess();
        if (current != nullptr) {
            this->createGenericResponse(current->getRemainingTime(), docOut);
        }
        else {
            this->createGenericResponse(0, docOut);
        }

        String response;
        serializeJson(docOut, response);

        //if (current == nullptr) {
            // messageHandler.ResetUnseenCounter();
        //}

        this->secureServer.send(200, "text/plain", response);
        Serial.println("Dump sent back!");
    }); 

    // handle posting simple value
    secureServer.on("/V", HTTP_POST, [this]() -> void {
        DynamicJsonDocument docOut(1024);
        StaticJsonDocument<512> docIn;

        if (!this->authenticationCheck()) {
            this->handleRedirectSecure();
            return;
        }

        DeserializationError err = deserializeJson(docIn, this->secureServer.arg("plain"));

        int wait = 0;
        int intValue = -1;

        if (err == DeserializationError::Ok) {
            JsonObject setting = docIn.as<JsonObject>();
            intValue = setting["V"].as<int>();
            float newValue = 1.0f * intValue / 100;
            processQueue.setClientValue(newValue);
            wait = 3;
        }
        else {
            messageHandler.addNewMessage({"J","S", err.c_str()}); 
            wait = 0;
        }

        createGenericResponse(wait, docOut);

        String response;
        serializeJson(docOut, response);

        secureServer.send(200, "text/plain", response);
        response = "";

        Serial.println("New value was posted: " + String(intValue));
    });

    //handle posting timings
    secureServer.on("/T", HTTP_POST, [this]() -> void {
        DynamicJsonDocument docOut(1024);
        StaticJsonDocument<768> docIn;

        if (!this->authenticationCheck()) {
            this->handleRedirectSecure();
            return;
        }

        DeserializationError err = deserializeJson(docIn, this->secureServer.arg("plain"));

        if (err == DeserializationError::Ok) {
            JsonObject timingConfig = docIn.as<JsonObject>();
            timingContainer.parseTimings(timingConfig);
            timingContainer.disableEarlierSettings();
            messageHandler.addNewMessage({"S", "T", "O"});
        } 
        else {
            this->messageHandler.addNewMessage({"J", "T", err.c_str()});
        }
        
        docIn.clear();
        esp_yield();

        createGenericResponse(0, docOut);

        String response;
        serializeJson(docOut, response);
        docOut.clear();

        esp_yield();

        this->secureServer.send(200, "text/plain", response);
        response = "";

        esp_yield();

        this->timingContainer.saveTimingsToFlash(this->secureServer.arg("plain"));
        Serial.println("Timings were updated!");
    });  

    //handle zeroing
    secureServer.on("/Z", HTTP_POST, [this]() -> void {
        DynamicJsonDocument docOut(1024);
        StaticJsonDocument<512> docIn;

        if (!this->authenticationCheck()) {
            this->handleRedirectSecure();
            return;
        }

        int8_t retryTime = 0;

        String zeroState = "";

        DeserializationError err = deserializeJson(docIn, this->secureServer.arg("plain"));
        esp_yield();

        if (err == DeserializationError::Ok) {
            JsonObject zeroObject = docIn.as<JsonObject>();
            zeroState = zeroObject["Z"].as<String>();

            if (zeroState == "find") {
                this->processQueue.processZero(ZeroState::find);
                retryTime = 3;
            }
            else {
                if (this->processQueue.getCurrentSettingProcess() != nullptr) {
                    this->messageHandler.addNewMessage({"Z", "F", "B"});
                    retryTime = 3;
                }
                else {
                    if(zeroState == "up") {
                        this->processQueue.processZero(ZeroState::up);
                        this->messageHandler.addNewMessage({"Z", "O", "U"});
                        retryTime = 0;
                    }

                    if(zeroState == "down") {
                        this->processQueue.processZero(ZeroState::down);
                        this->messageHandler.addNewMessage({"Z", "O", "D"});
                        retryTime = 0;
                    }
                }
            }
        }
        else {
            this->messageHandler.addNewMessage({"J","Z", err.c_str()});
            retryTime = 0;
        }

        createGenericResponse(retryTime, docOut);
        String response;
        serializeJson(docOut, response);
        esp_yield();
        this->secureServer.send(200, "text/plain", response);

        Serial.println("Zeroing was set: " + zeroState);
    });

    //handle login
    secureServer.on("/L", HTTP_POST, [this]() -> void {
        if (this->secureServer.hasHeader("Cookie") && isCookieValid()) {
            Serial.println("ALREADY SIGNED IN!");
            //if (this->secureServer.hasHeader("Cookie") && this->isCookieValid()) {
                this->secureServer.send(200, "text/html", controlPage);
            //}
            //else {
                //this->secureServer.send(200, "text/html", loginPage);
                //Serial.println("Showing login page!");
            //}
        }
        else {
            if (this->authenticationCheck()) {
                Serial.println("Signing IN...");
                this->secureServer.sendHeader("Cache-Control", "no-cache");
                this->secureServer.sendHeader("Set-Cookie", generateCookie());
                this->secureServer.sendHeader("Location", "/");
                this->secureServer.send(302, "text/html", "");
            }
            else {
                this->secureServer.send(200, "text/html", loginFail);
            }    
        }
    });

    //handle logout
    secureServer.on("/O", HTTP_GET, [this]() -> void {
        Serial.println("Signing out...");
        this->secureServer.sendHeader("Set-Cookie", "session=null");
        this->secureServer.sendHeader("Location", "/");
        this->secureServer.send(302, "text/html", "");
    });

    //handle unknown address
    secureServer.onNotFound([this]() -> void {
        this->secureServer.sendHeader("Location", String("/"), true);
        this->secureServer.send(302, "text/plain", "");
        Serial.println("Redirect to root secure!");
    });
    
    secureServer.collectHeaders("Cookie");
    secureServer.begin();

    //setup not secure redirecting server
    server.on("/", HTTP_GET, [this]() -> void {
        this->server.sendHeader("Location", this->secureAddress, true);
        this->server.send(302, "text/plain", "");
        Serial.println("Redirect to secure!");
    });

    server.onNotFound([this]() -> void {
        this->server.sendHeader("Location", this->secureAddress, true);
        this->server.send(302, "text/plain", "");
        Serial.println("Redirect to secure!");
    });

    server.begin();
}

void ServerContainer::listen() {
    server.handleClient();
    secureServer.handleClient();
    processQueue.processQueue();
}

void ServerContainer::handleRedirectSecure() {
    this->secureServer.sendHeader("Location", String("/"), true);
    this->secureServer.send(302, "text/plain", "");
    Serial.println("Redirect to root secure!");
}

void ServerContainer::createGenericResponse(int waitTime, JsonDocument& document) {
    JsonObject genericResponse = document.createNestedObject("G");
    
    if (waitTime > 0) {
        genericResponse["R"] = waitTime;
        return;
    }

    JsonObject messageObject = genericResponse.createNestedObject("M");
    messageHandler.getEveryMessage(messageObject);

    esp_yield();

    genericResponse["V"] = (int) (processQueue.getCurrentValue() * 100);
    genericResponse["R"] = waitTime;
}

bool ServerContainer::authenticationCheck() {
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

bool ServerContainer::isCookieValid() {
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

String ServerContainer::generateCookie() {
    lastCookieUsage = millis();
    currentCookie = "session=" + sha1(USER_NAME + String(lastCookieUsage));
    cookieStartTime = millis();

    return currentCookie;
}