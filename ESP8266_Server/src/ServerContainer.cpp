#include "ServerContainer.h"

#include "../Keys.h"
#include "BrowserClient.h"
#include "Hash.h"

#include "pb_encode.h"
#include "pb_decode.h"

void ServerContainer::initialize() {
    messageHandler.initialize();
    processQueue.initialize();
    timingContainer.initialize(&processQueue);

    processQueue.setMessageProcessor([this](Shutter_Event event, int value) -> void {
        this->messageHandler.addNewMessage(event, value);
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

    secureServer.on("/format", HTTP_GET, [this]() -> void {
        if(this->authenticationCheck()) {
            LittleFSHandler::format();

            this->secureServer.send(200, "text/plain", "Server formatted and restarted!");
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

    secureServer.on("/Shutter.proto", HTTP_GET, [this]() -> void {
        this->secureServer.sendHeader("Cross-Origin-Embedder-Policy", "require-corp");
        this->secureServer.sendHeader("Cache-Control", "max-age=31536000");        
        this->secureServer.send(200, "text/plain", StaticPage::protoFormat);
    });

    secureServer.on("/W.js", HTTP_GET, [this]() -> void {
        this->secureServer.sendHeader("Cross-Origin-Embedder-Policy", "require-corp");
        this->secureServer.sendHeader("Cache-Control", "max-age=31536000");        
        this->secureServer.send(200, "application/javascript", StaticPage::wasmWorker);
    });

    //handle getting the current value, message history
    secureServer.on("/S", HTTP_GET, [this]() -> void {
        if (this->authenticationCheck()) {
            SettingProcess* current = this->processQueue.getCurrentSettingProcess();
            uint8_t buffer[Shutter_Response_size];
            Shutter_Response response = Shutter_Response_init_default;

            int bytesWritten = this->serializeResponseContent(buffer, response, current != nullptr ? current->getRemainingTime() : 0);
            this->secureServer.send(200, "text/plain", buffer, bytesWritten);

            Serial.println("Status was sent!");   
        }
        else {
            this->handleRedirectSecure();
        }
    });

    // handle getting the dump: current value, message history and the timings
    secureServer.on("/D", HTTP_GET, [this]() -> void {
        if (this->authenticationCheck()) {
            uint8_t buffer[Shutter_TimingContainer_size];
            Shutter_TimingContainer timingContainerResponse = Shutter_TimingContainer_init_default;
            timingContainerResponse.timing_count = NR_OF_TIMINGS;

            for (int i = 0; i < NR_OF_TIMINGS; i++) {
                timingContainerResponse.timing[i] = timingContainer.getTiming(i).getTiming();
            }

            pb_ostream_t stream = pb_ostream_from_buffer(buffer, Shutter_TimingContainer_size);    
            if (!pb_encode(&stream, Shutter_TimingContainer_fields, &timingContainerResponse)) {
                Serial.println("Failed to encode out timings..");
            }

            this->secureServer.send(200, "text/plain", buffer, stream.bytes_written);
            Serial.println("Dump sent back!");
        }
        else {
            this->handleRedirectSecure();
        }
    }); 

    // handle posting simple value
    secureServer.on("/V", HTTP_POST, [this]() -> void {
        if (this->authenticationCheck()) {
            String content = this->secureServer.arg("plain");
            Shutter_Request request = Shutter_Request_init_default; 
            this->parseRequestContent(content, request);

            request.value = request.value > 100 ? 100 : (request.value < 0 ? 0 : request.value);
            request.value -= request.value % 5 - (request.value % 5 < 3 ? 0 : 5);

            processQueue.setClientValue(1.0f * request.value / 100);
            int wait = 3;

            uint8_t buffer[Shutter_Response_size];
            Shutter_Response response = Shutter_Response_init_default;
            SettingProcess* current = processQueue.getCurrentSettingProcess();

            int bytesWritten = this->serializeResponseContent(buffer, response, wait);
            this->secureServer.send(200, "text/plain", buffer, bytesWritten);
        }
        else {
            this->handleRedirectSecure();
        }       
    });

    //handle posting timings
    secureServer.on("/T", HTTP_POST, [this]() -> void {
        if (this->authenticationCheck()) {
            String content = this->secureServer.arg("plain");
            Shutter_Request request = Shutter_Request_init_default; 
            this->parseRequestContent(content, request);

            timingContainer.parseTimings(request);
            timingContainer.disableEarlierSettings();
            messageHandler.addNewMessage(Shutter_Event_timings_updated, 0);

            uint8_t buffer[Shutter_Response_size];
            Shutter_Response response = Shutter_Response_init_default;

            int bytesWritten = this->serializeResponseContent(buffer, response);
            this->secureServer.send(200, "text/plain", buffer, bytesWritten);

            esp_yield();
            LittleFSHandler::writeFile("timings.txt", (uint8_t*) content.c_str(), content.length());
            Serial.println("Timings were updated!");
        }
        else {
            this->handleRedirectSecure();
        }        
    });  

    //handle zeroing
    secureServer.on("/Z", HTTP_POST, [this]() -> void {
        if (this->authenticationCheck()) {
            String content = this->secureServer.arg("plain");
            Shutter_Request request = Shutter_Request_init_default; 
            this->parseRequestContent(content, request);

            int8_t retryTime = 0;
            
            if (request.zero == Shutter_Zero_current) {
                this->processQueue.processZero(request.zero);
                retryTime = 3;
            }
            else if (this->processQueue.getCurrentSettingProcess() != nullptr) {
                this->messageHandler.addNewMessage(Shutter_Event_zero_fail, 0);
                retryTime = this->processQueue.getCurrentSettingProcess()->getRemainingTime();
            }
            else if(request.zero == Shutter_Zero_up) {
                this->processQueue.processZero(request.zero);
                this->messageHandler.addNewMessage(Shutter_Event_zero, 100);
                retryTime = 0;
            }
            else if(request.zero == Shutter_Zero_down) {
                this->processQueue.processZero(request.zero);
                this->messageHandler.addNewMessage(Shutter_Event_zero, 0);
                retryTime = 0;
            }
    
            uint8_t buffer[Shutter_Response_size];
            Shutter_Response response = Shutter_Response_init_default;

            int bytesWritten = this->serializeResponseContent(buffer, response, retryTime);
            this->secureServer.send(200, "text/plain", buffer, bytesWritten);

            Serial.println("Zeroing was set: " + String(request.zero));
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

bool ServerContainer::parseRequestContent(String& content, Shutter_Request& request) {
    pb_istream_t istream = pb_istream_from_buffer((const unsigned char*) content.c_str(), content.length());
    return pb_decode(&istream, Shutter_Request_fields, &request);
}

int ServerContainer::serializeResponseContent(uint8_t* buffer, Shutter_Response& response, int waitTime) {
    response.retryTime = waitTime;

    if (waitTime == 0) {
        response.value = (int) (processQueue.getCurrentValue() * 100);
        Shutter_MessageContainer& messageContainer = messageHandler.getMessageContainer();
        response.has_messageContainer = true;
        response.messageContainer = messageContainer;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, Shutter_Response_size);
    
    if (!pb_encode(&stream, Shutter_Response_fields, &response)) {
        Serial.println("Failed to encode generic response...");
        return 0;
    }

    esp_yield();

    return stream.bytes_written;
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