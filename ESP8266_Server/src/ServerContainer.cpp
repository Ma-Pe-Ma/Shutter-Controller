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
            this->secureServer.send(200, "text/html", StaticPage::controlPage);
        }
        else {
            this->secureServer.send(200, "text/html", StaticPage::loginPage);
            Serial.println("Showing login page!");
        }
    });

    secureServer.on("/R", HTTP_GET, [this]() -> void {
        if(this->authenticationCheck()) {
            this->secureServer.send(200, "text/plain", "Server restarted!");
            ESP.restart();
        }
        else{
            this->handleRedirectSecure();
        }        
    });

    secureServer.on("/favicon.ico", HTTP_GET, [this]() -> void {
        this->secureServer.sendHeader("Location", RESOURCE_PROVIDER "/favicon.ico");
        this->secureServer.send(301, "image/x-icon", "");
    });

    secureServer.on("/W", HTTP_GET, [this]() -> void {
        if (this->authenticationCheck()) {
            this->secureServer.sendHeader("Cross-Origin-Opener-Policy", "same-origin");
            this->secureServer.sendHeader("Cross-Origin-Embedder-Policy", "require-corp");
            this->secureServer.sendHeader("Cache-Control", "max-age=31536000");
            this->secureServer.send(200, "text/html", StaticPage::wasmPage);
        }
        else {
            this->handleRedirectSecure();
        }       
    });

    secureServer.on("/W.js", HTTP_GET, [this]() -> void {
        this->secureServer.sendHeader("Cross-Origin-Embedder-Policy", "require-corp");
        this->secureServer.sendHeader("Cache-Control", "max-age=31536000");        
        this->secureServer.send(200, "application/javascript", StaticPage::wasmWorker);
    });

    //handle getting the current value, message history
    secureServer.on("/S", HTTP_GET, [this]() -> void {
        if (this->authenticationCheck()) {
            DynamicJsonDocument docOut(1024);

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
        }
        else {
            this->handleRedirectSecure();
        }
    });

    // handle getting the dump: current value, message history and the timings
    secureServer.on("/D", HTTP_GET, [this]() -> void {
        if (this->authenticationCheck()) {
            DynamicJsonDocument docOut(2048);

            String timingsString;
            this->timingContainer.readTimingsFromFlash(timingsString);

            if (timingsString != "") {
                docOut["T"] = serialized(timingsString.c_str());
            }            
            
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
        }
        else {
            this->handleRedirectSecure();
        }
    }); 

    // handle posting simple value
    secureServer.on("/V", HTTP_POST, [this]() -> void {
        if (this->authenticationCheck()) {
            DynamicJsonDocument docOut(1024);
            StaticJsonDocument<512> docIn;

            DeserializationError err = deserializeJson(docIn, this->secureServer.arg("plain"));

            int wait = 0;
            int intValue = -1;

            if (err == DeserializationError::Ok) {
                JsonObject setting = docIn.as<JsonObject>();
                intValue = setting["V"].as<int>();

                int digit = intValue % 5;

                if (digit != 0)
                {
                    intValue = digit < 3 ? intValue - digit : intValue - digit + 5;
                }

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
    
            this->secureServer.send(200, "text/plain", response);
            response = "";

            Serial.println("New value was posted: " + String(intValue));
        }
        else {
            this->handleRedirectSecure();
        }       
    });

    //handle posting timings
    secureServer.on("/T", HTTP_POST, [this]() -> void {
        if (this->authenticationCheck()) {
            DynamicJsonDocument docOut(1024);
            StaticJsonDocument<768> docIn;

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
        }
        else {
            this->handleRedirectSecure();
        }        
    });  

    //handle zeroing
    secureServer.on("/Z", HTTP_POST, [this]() -> void {
        if (this->authenticationCheck()) {
            DynamicJsonDocument docOut(1024);
            StaticJsonDocument<512> docIn;    
            
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
        }
        else {
            this->handleRedirectSecure();
        }        
    });

    //handle login
    secureServer.on("/L", HTTP_POST, [this]() -> void {
        //if already signed in
        if (this->secureServer.hasHeader("Cookie") && isCookieValid()) {
            Serial.println("ALREADY SIGNED IN!");
            this->secureServer.sendHeader("Location", String("/"));
            this->secureServer.send(302, "text/plain", "");
        }
        else {
            if (this->authenticationCheck()) {
                Serial.println("Signing IN...");
                this->secureServer.sendHeader("Cache-Control", "no-cache");
                this->secureServer.sendHeader("Set-Cookie", generateCookie());
                this->secureServer.sendHeader("Location", String("/"));
                this->secureServer.send(302, "text/plain", "");
            }
            else {
                this->secureServer.send(200, "text/html", StaticPage::loginFail);
            }    
        }
    });

    //handle logout
    secureServer.on("/O", HTTP_GET, [this]() -> void {
        Serial.println("Signing out...");
        this->secureServer.sendHeader("Set-Cookie", "session=null");
        this->secureServer.sendHeader("Location", String("/"), true);
        this->secureServer.send(302, "text/plain", "");
    });

    //handle unknown address
    secureServer.onNotFound([this]() -> void {
        this->secureServer.sendHeader("Location", String("/"), true);
        this->secureServer.send(302, "text/plain", "");
        Serial.println("Secure address not found, redirect to root secure!");
    });
    
    secureServer.collectHeaders("Cookie");
    secureServer.begin();

    //setup not secure redirecting server
    server.on("/", HTTP_GET, [this]() -> void {
        this->server.sendHeader("Location", this->secureAddress, true);
        this->server.send(302, "text/plain", "");
        Serial.println("Insecure root called, redirect to secure root!");
    });

    server.onNotFound([this]() -> void {
        this->server.sendHeader("Location", this->secureAddress, true);
        this->server.send(302, "text/plain", "");
        Serial.println("Insecure address not found, redirect to secure!");
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
#ifdef DISABLE_AUTHENTICATION
    secureServer.sendHeader("Access-Control-Allow-Origin", "*");
    return true;
#endif

    if (isCookieValid()) {
        return true;
    }

    if (secureServer.args() < 2) {
        return false;
    }

#ifndef COOKIE_AUTHORIZATION_ONLY
    if (secureServer.hasArg("username") && secureServer.hasArg("password")) {
        if (secureServer.arg("username") == USER_NAME && secureServer.arg("password") == PASSWORD) {
            return true;
        }
    }
#endif

    return false;
}

bool ServerContainer::isCookieValid() {
    String receivedCookie = secureServer.header("Cookie");

    //Sign out after 5 minutes of idle state
    if (millis() - lastCookieUsage >  5 * 60 * 1000) {
        return false;
    }

    //Force sign out 60 minutes after login
    if (millis() - cookieStartTime > 60 * 60 * 1000) {
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