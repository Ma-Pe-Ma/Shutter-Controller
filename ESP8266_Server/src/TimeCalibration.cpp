#include "TimeCalibration.h"
#include <Time.h>

#define REBOOT_INTERVAL 48

namespace TimeCalibration {
    WiFiUDP ntpUDP;
    NTPClient dateTime = NTPClient(ntpUDP, NTP_ADRESS, TIME_ZONE * 3600, UPDATE_FREQ * 1000);

    const unsigned long day48time = 3600 * 24 * 48;

    //offset by one, to start week on monday
    inline int8_t CorrectDay(int8_t day) {
        int8_t correctedDay = day - 1;

        if (correctedDay == -1) {
            correctedDay = 6;
        }
        
        return correctedDay;
    }
    
    void GetCurrentTime(int8_t& day, int8_t& hour, int8_t& minute) {
        time_t epochTime = dateTime.getEpochTime();
        struct tm* ti = gmtime(&epochTime);

        day = CorrectDay(ti->tm_wday);
        hour = ti->tm_hour;
        minute = ti->tm_min;
    }    

    void InitializeDateTime() {
        dateTime.begin();
        dateTime.forceUpdate();

        time_t epochTime = dateTime.getEpochTime();
        struct tm *ptm = gmtime ((time_t *) & epochTime);

        int year = ptm->tm_year + 1900;

        //if timefetch was unsuccesful, recursively try again!
        if (year < 2021) {
            InitializeDateTime();
        }
        else {
            CorrectByDST();
        }

        Serial.println("Time after initializing: " + GetFormattedString());
    } 

    void Update() {
        //Maybe disable update if currentProcess is in progress??
        if (dateTime.update()) {
            CorrectByDST();
            Serial.println("Time updated: " + GetFormattedString());

            Timing::CheckTimings(CorrectDay(dateTime.getDay()), dateTime.getHours(), dateTime.getMinutes());

            if (millis() > day48time) {
                ESP.restart();
            }
        }
    }

    void CorrectByDST() {        
        dateTime.setTimeOffset((TIME_ZONE) * 3600);

        time_t epochTime = dateTime.getEpochTime();
        struct tm* ti = gmtime(&epochTime);

        int month = ti->tm_mon;
        int day = ti->tm_mday;
        int weekDay = ti->tm_wday;
        int hours = ti->tm_hour;

        if (IsDST(month, day, weekDay, hours)) {
            dateTime.setTimeOffset((TIME_ZONE + 1) * 3600);
        }
    }

    bool IsDST(int month, int day, int dayOfWeek, int hour) {
        if (month < 2 || 9 < month) {
            return false;
        }
        else if (2 < month && month < 9) {
            return true;
        }
        else if (month == 2) {
            int lastSundayOfMonth = GetLastSundayOfMonth(day, dayOfWeek);

            if (day < lastSundayOfMonth) {
                return false;
            }
            else if (day > lastSundayOfMonth) {
                return true;
            }
            else {
                if (hour > 1) {
                    return true;
                }
                else {
                    return false;
                }
            }
        }
        else if (month == 9) {
            int lastSundayOfMonth = GetLastSundayOfMonth(day, dayOfWeek);

            if (day < lastSundayOfMonth) {
                return true;
            }
            else if (day > lastSundayOfMonth) {
                return false;
            }
            else {
                if (hour >= 2) {
                    return false;
                }
                else {
                    return true;
                }
            }
        }

        return false;
    }

    int GetLastSundayOfMonth(int day, int dayOfWeek) {
        int lastSunday = day - dayOfWeek;

        while (lastSunday + 7 < 32) {
            lastSunday += 7;
        }

        return lastSunday;
    }

    String GetFormattedString() {
        return GetFormattedStringByEpoch(dateTime.getEpochTime());
    }

    String GetFormattedStringByEpoch(time_t epochTime) {
        struct tm *ptm = gmtime ((time_t *) & epochTime);

        int year = ptm->tm_year;
        String month = String(ptm->tm_mon + 1);
        month = month.length() == 1 ? "0" + month : month;
        String day = String(ptm->tm_mday);
        day = day.length() == 1 ? "0" + day : day;

        String hours = String(ptm->tm_hour);
        hours = hours.length() == 1 ? "0" + hours : hours;

        String minutes = String(ptm->tm_min);
        minutes = minutes.length() == 1 ? "0" + minutes : minutes;

        return String(year + 1900) + ". " + month + ". " + day + ". " + hours + ":" + minutes;
    }

    time_t CustomTimeSetter() {
        return dateTime.getEpochTime();
    }
}