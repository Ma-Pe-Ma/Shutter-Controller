#ifndef SERVERCONTAINER_H
#define SERVERCONTAINER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>

#include "MessageHandler.h"
#include "TimeCalibration.h"

#include "Processes/TimingContainer.h"
#include "Processes/ProcessQueue.h"
#include "Processes/ZeroProcess.h"

#include "Shutter.pb.h"

#include "../Configuration.h"

class MessageHandler;

class ServerContainer {
private:
    ESP8266WebServer server = ESP8266WebServer(HTTP_PORT);
    BearSSL::ESP8266WebServerSecure secureServer = BearSSL::ESP8266WebServerSecure(HTTPS_PORT);    

    TimingContainer timingContainer;
    ProcessQueue processQueue;
    MessageHandler messageHandler;

    const String secureAddress = "https://" + String(DDNS_DOMAIN);

    void handleRedirectSecure();

    bool parseRequestContent(String& content, Shutter_Request request);
    int serializeResponseContent(uint8_t* buffer, Shutter_Response&, int waitTime = 0);

    unsigned long lastCookieUsage = 0;
    unsigned long cookieStartTime = 0;
    String currentCookie;

    bool authenticationCheck();
    String generateCookie();
    bool isCookieValid();
public:   
    void initialize();
    void listen();

    MessageHandler& getMessageHandler() {return this->messageHandler; }
    ProcessQueue& getProcessQueue() {return this->processQueue; }
    TimingContainer& getTimingContainer() {return this->timingContainer; }
};

#endif