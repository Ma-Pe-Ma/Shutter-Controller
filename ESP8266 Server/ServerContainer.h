#ifndef SERVERCONTAINER_H
#define SERVERCONTAINER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <ArduinoJson.h>
#include "TimingSetter.h"
#include "NullProcess.h"
#include "MessageHandler.h"
#include "TimeCalibration.h"

extern const char serverCert[];
extern const char serverKey[];

namespace ServerContainer {
    enum ResponseType {
        OK,
        WAIT,
        ERROR
    };

    extern bool blackout;

    void handleRoot();
    void handleStatus();
    void handleSetting();
    void handleScheduling();
    void handleUnknown();
    void handleZero();
    void handleDump();
 
    extern BearSSL::ESP8266WebServerSecure secureServer;
    void Initialize();
    void SendResponse(ResponseType);
    String CreateNewMessage(String, String, String);
}

#endif