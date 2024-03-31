#ifndef DDNSUPDATER_H
#define DDNSUPDATER_H

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

namespace DdnsUpdater {

    inline String domain;
    inline String token;
    inline String oldIP;

    inline unsigned long previousTime = 0;

    inline void init(String domain, String token) {
        DdnsUpdater::domain = domain;
        DdnsUpdater::token = token;
    }

    inline String getNewIP() {
        String newIP;

        BearSSL::WiFiClientSecure client;
        client.setInsecure();
        HTTPClient http;
        http.begin(client, "https://ifconfig.me/ip");
        int httpCode = http.GET();

        if (httpCode > 0) {
            if (httpCode == HTTP_CODE_OK) {
                newIP = http.getString();                
            }
        } else {
            http.end();
            return DdnsUpdater::oldIP;
        }
        http.end();
        
        return newIP;
    }

    inline void updateIP(String newIP) {
        BearSSL::WiFiClientSecure client;
        client.setInsecure();

        HTTPClient http;
        http.begin(client, "https://www.duckdns.org/update?domains=" + domain + "&token=" + token + "&ip=" + newIP + "");

        int httpCode = http.GET();
        if (httpCode == 200 && http.getString() == "OK") {
            Serial.println("New IP: " + newIP);
            DdnsUpdater::oldIP = newIP;
        }
        http.end();
    }

    inline void update(unsigned long intervalTime) {
        unsigned long currentTime = millis(); // Calculate Elapsed Time & Trigger
        if (currentTime - previousTime >= intervalTime) {
            previousTime = currentTime;

            String newIP = getNewIP();

            if (DdnsUpdater::oldIP != newIP) {
                updateIP(newIP);
            }
        }
    }
}

#endif