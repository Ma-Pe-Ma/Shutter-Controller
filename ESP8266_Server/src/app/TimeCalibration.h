#ifndef TIMECALIBRATION_H
#define TIMECALIBRATION_H

#include <NTPClient.h>
#include <WiFiUdp.h>

#include "Processes/Timing.h"

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
    time_t customTimeSetter();

    void getCurrentTime(int&, int&, int&);
    void getDatetime(int32_t& year, int32_t& month, int32_t& day, int32_t& hour, int32_t& minute);
}

#endif