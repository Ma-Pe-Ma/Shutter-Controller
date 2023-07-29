#ifndef TIMECALIBRATION_H
#define TIMECALIBRATION_H

#include <NTPClient.h>
#include <WiFiUdp.h>

#include "Processes/Timing.h"

#include <Time.h>

//UTC+TIME_ZONE
#define TIME_ZONE 1
#define NTP_ADRESS "pool.ntp.org"
#define UPDATE_FREQ 30 //in secs

namespace TimeCalibration {
    extern WiFiUDP ntpUDP;
    extern NTPClient dateTime;
    extern bool dst;    

    int8_t correctDay(int8_t day);

    bool update();
    void initializeDateTime();
    void correctByDST();
    bool isDST(int, int, int, int);
    int getLastSundayOfMonth(int, int);
    String getFormattedString();
    String getFormattedStringByEpoch(time_t);
    time_t customTimeSetter();

    void getCurrentTime(int8_t&, int8_t&, int8_t&);
}

#endif