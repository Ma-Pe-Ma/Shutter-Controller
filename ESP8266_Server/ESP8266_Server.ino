#include "ServerContainer.h"
#include <NTPClient.h>

#include "SettingProcess.h"
#include "Timing.h"
#include "TimeCalibration.h"
#include "MessageHandler.h"

#define SSID ""
#define PSWD ""

//TODO:
//      DDNS
//      TITKOSÍTÁS
//      ZEROING AT SETUP
//      COMMUNICATION?
//      SERVER LOGGER?
//      DEFAULT HTML PAGE?
//      WIFI REJOIN?
//      TRY: LARGEST MESSAGE: 10*timing + 10* message

void setup() {
    Serial.begin(115200);
    Serial.println("");

    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    
    WiFi.begin(SSID, PSWD);

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    //ZEROING?
    MessageHandler::Initialize();
    TimeCalibration::InitializeDateTime();
    ServerContainer::Initialize();
}

void loop() {
    //serverHTTP.handleClient();
    ServerContainer::secureServer.handleClient();

    TimeCalibration::Update();
    SettingProcess::processQueue();
}