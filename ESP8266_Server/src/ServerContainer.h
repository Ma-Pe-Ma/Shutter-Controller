#ifndef SERVERCONTAINER_H
#define SERVERCONTAINER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <ArduinoJson.h>
#include "Processes/ZeroProcess.h"
#include "MessageHandler.h"
#include "TimeCalibration.h"

namespace ServerContainer {
    void handleRoot();
    void handleRedirect();
    void handleRedirectSecure();
    
    void handleStatus();
    void handleGetDump();

    void handlePostSetting();    
    void handlePostTiming();
    void handlePostZero();
 
    extern BearSSL::ESP8266WebServerSecure secureServer;
    extern ESP8266WebServer server;
    void initialize();
    void createGenericResponse(int retryTime, bool root = false);
    bool authenticationCheck();
}

#endif