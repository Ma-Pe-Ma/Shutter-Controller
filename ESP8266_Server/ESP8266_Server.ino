#include "src/ServerContainer.h"
#include <NTPClient.h>
#include <EasyDDNS.h>

#include "src/LittleFSHelper.h"
#include "src/Processes/SettingProcess.h"
#include "src/Processes/Timing.h"
#include "src/TimeCalibration.h"
#include "src/MessageHandler.h"

#include "Configuration.h"

void reconnect();

void setup() {
    delay(5000);

    Serial.begin(115200);
    Serial.println("Program started!");
    Serial.println("");

    pinMode(UP_PIN, OUTPUT);
    pinMode(DOWN_PIN, OUTPUT);
    digitalWrite(UP_PIN, LOW);
    digitalWrite(DOWN_PIN, LOW);
    
    WiFi.mode(WIFI_STA);

    WiFi.begin(SSID, PSWD);

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");

    Serial.print("Connected, IP address: ");
    Serial.println( WiFi.localIP());

    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());

    TimeCalibration::InitializeDateTime();
    Serial.println("Current date-time: " + TimeCalibration::GetFormattedString());

    LittleFSHelper::initialize();
    MessageHandler::Initialize();
    SettingProcess::initialize();
    Timing::initialize();
    ServerContainer::initialize();

    String date = String(SettingProcess::getLastSetDay()) + "-" + String(SettingProcess::getLastSetHour()) + ":"+String(SettingProcess::getLastSetMin());
    Serial.println("Startup value: " + String(SettingProcess::getCurrentValue()) +", date:" + date);
    Serial.println();

    EasyDDNS.service(DDNS_SERVICE);
    EasyDDNS.client(DDNS_DOMAIN, DDNS_TOKEN);

    EasyDDNS.onUpdate([&] (const char* oldIP, const char* newIP) {
        Serial.print("New IP: ");
        Serial.println(newIP);
    });

    Serial.println("STARTUP FINISHED!");
}

unsigned long lasttime = 0;

void loop() {
    unsigned long current = millis();    

    if (WiFi.status() != WL_CONNECTED && SettingProcess::getQueueCount() == 0) {
        reconnect();
    }
    else if (WiFi.status() != WL_CONNECTED && SettingProcess::getQueueCount() != 0) {
        SettingProcess::processQueue();  

        if (current - lasttime > 3000) {
            lasttime = current;
            Serial.println("Not connected, but processing: " + String(SettingProcess::getQueueCount()));
        }
    }
    else {
        if (current - lasttime > 30000) {
            lasttime = current;
            Serial.println("UP AND RUNNING WAITING");
        }

        TimeCalibration::Update();
        EasyDDNS.update(10000);
        ServerContainer::server.handleClient();
        ServerContainer::secureServer.handleClient();
        SettingProcess::processQueue();   
    } 

    yield();
}

void reconnect() {
    Serial.println("Reconnecting");

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PSWD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
}