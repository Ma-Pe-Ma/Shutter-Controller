#include "TimeCalibration.h"

namespace TimeCalibration {
    WiFiUDP ntpUDP;
    NTPClient dateTime = NTPClient(ntpUDP, NTP_ADRESS, TIME_ZONE * 3600, UPDATE_FREQ * 1000);
    bool dst = false;

    void Update() {
         //Maybe disable update if currentProcess is in progress??
        if (dateTime.update()) {
            SetCorrectTime();
            Timing::CheckTimings(dateTime.getDay(), dateTime.getHours(), dateTime.getMinutes());
        }
    }

    void InitializeDateTime() {
        dateTime.forceUpdate();
        SetCorrectTime();
    }

    void SetCorrectTime() {
        dateTime.setTimeOffset((TIME_ZONE) * 3600);
        unsigned long epochTime = dateTime.getEpochTime();
        struct tm *ptm = gmtime ((time_t *) & epochTime);

        int month = ptm->tm_mon;
        int day = ptm->tm_mday;
        int weekDay = ptm->tm_wday;

        dst = IsDST(month, day, weekDay, dateTime.getHours());

        if (dst) {
            dateTime.setTimeOffset((TIME_ZONE + 1) * 3600);
        }
    }

    bool IsDST(int month, int day, int dayOfWeek, int hour) {
        if (month <2 || 9 < month) {
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
                if (hour >=2) {
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
                if (hour >=2) {
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
        unsigned long epochTime = dateTime.getEpochTime();
        struct tm *ptm = gmtime ((time_t *) & epochTime);

        int year = ptm->tm_year;
        int month = ptm->tm_mon;
        int day = ptm->tm_mday;

        return String(year) + "-" + String(month) + "-" +String(day) + " " + dateTime.getFormattedTime();
    }
}