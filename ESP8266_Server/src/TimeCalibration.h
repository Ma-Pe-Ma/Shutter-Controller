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

    void Update();
    void InitializeDateTime();
    void CorrectByDST();
    bool IsDST(int, int, int, int);
    int GetLastSundayOfMonth(int, int);
    String GetFormattedString();
    String GetFormattedStringByEpoch(time_t);
    time_t CustomTimeSetter();

    void GetCurrentTime(int8_t&, int8_t&, int8_t&);
}

#endif