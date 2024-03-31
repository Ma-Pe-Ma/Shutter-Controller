#include <NTPClient.h>

#include "app/ServerContainer.h"
#include "app/LittleFSHandler.h"
#include "app/Processes/SettingProcess.h"
#include "app/Processes/Timing.h"
#include "app/TimeCalibration.h"
#include "app/MessageHandler.h"
#include "app/DdnsUpdater.h"

#include "Configuration.h"

ServerContainer serverContainer;

void setup() {
    // setup pins
    //This is needed when rx/gpio3 pin is used!
    //pinMode(UP_PIN, FUNCTION_3);
    pinMode(UP_PIN, OUTPUT); 
    digitalWrite(UP_PIN, DEACTIVATE_PIN);

    pinMode(DOWN_PIN, OUTPUT);
    digitalWrite(DOWN_PIN, DEACTIVATE_PIN);

    delay(1000);

    // setup serial
    //Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
    Serial.begin(115200);
    //Serial.setDebugOutput(true);
    Serial.println("Program started!");
    Serial.println("");
    
    // setup wifi
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

    // setup tools and the server itself
    TimeCalibration::initializeDateTime();
    LittleFSHandler::initialize(FORMAT_FS_ONSTARTUP);
    serverContainer.initialize();
    LittleFSHandler::deleteFile("fail.txt");

    float value = serverContainer.getProcessQueue().getCurrentValue();
    String date = serverContainer.getProcessQueue().getLastSetDate();

    Serial.println("Startup value: " + String(value) + ", date:" + date);
    Serial.println();

    DdnsUpdater::init(DDNS_DOMAIN, DDNS_TOKEN);

    Serial.println("STARTUP FINISHED!");
}

unsigned long lasttime = 0;

void loop() {
    unsigned long current = millis();

    //if WiFi is connected then act normally
    if (WiFi.status() == WL_CONNECTED) {
        //notifying current working state on serial
        if (current - lasttime > 30000) {
            lasttime = current;
            Serial.println("UP AND RUNNING WAITING");
        }

        // update time every 30 secs and enqueue a scheduled timing if it is neccessary
        if(TimeCalibration::update()) {
            serverContainer.getTimingContainer().checkTimings(TimeCalibration::correctDay(TimeCalibration::dateTime.getDay()), TimeCalibration::dateTime.getHours(), TimeCalibration::dateTime.getMinutes());
        }
        
        if (serverContainer.getProcessQueue().getQueueCount() == 0) {
            DdnsUpdater::update(30000);
        }
        
        //handle server + setting functionalities
        serverContainer.listen();
    }
    else {
        // if disconnected and no process is in progress, try to reccnnect
        if (serverContainer.getProcessQueue().getQueueCount() == 0) {
            Serial.println("Reconnecting");

            WiFi.mode(WIFI_STA);
            WiFi.begin(SSID, PSWD);

            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
            }

            Serial.println();

            Serial.print("Reconnected, IP address: ");
            Serial.println(WiFi.localIP());
        }
        // if disconnected then first process the current process
        else {
            serverContainer.getProcessQueue().processQueue();

            if (current - lasttime > 3000) {
                lasttime = current;
                Serial.println("Not connected, but processing: " + String(serverContainer.getProcessQueue().getQueueCount()));
            }
        }
    }

    esp_yield();
}