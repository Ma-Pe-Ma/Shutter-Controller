package com.example.rednykapcsol;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;

import org.joda.time.LocalTime;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class Timing {
    public static final int numberOfSlots = 12;
    private static Timing[] timings = new Timing[numberOfSlots];
    private static Map<WeekDay, String> dayID;

    private boolean active;
    private int value = 100;
    private LocalTime time;
    private Map<WeekDay, Boolean> days;

    public static void initialize(Activity activity) {
        dayID = new HashMap<>();

        dayID.put(WeekDay.MONDAY, "H");
        dayID.put(WeekDay.TUESDAY, "K");
        dayID.put(WeekDay.WEDNESDAY, "Sze");
        dayID.put(WeekDay.THURSDAY, "Cs");
        dayID.put(WeekDay.FRIDAY, "P");
        dayID.put(WeekDay.SATURDAY, "Szo");
        dayID.put(WeekDay.SUNDAY, "V");

        for (int i = 0; i < timings.length; i++) {
            timings[i] = new Timing();
        }

        SharedPreferences sharedPref = activity.getPreferences(Context.MODE_PRIVATE);
        String key = activity.getResources().getString(R.string.timing_pref);
        String timingString = sharedPref.getString(key, "");
        parseTimingObjectDump(timingString);
    }

    public Timing() {
        time = null;
        days = new HashMap<>();

        for (WeekDay day : WeekDay.values()) {
            days.put(day, false);
        }

        value = 100;
    }

    public Timing(int value, Map<WeekDay, Boolean> days, LocalTime time) {
        this.value = value;
        this.days = days;
        this.time = time;
    }

    public boolean isActive() {
        return active;
    }

    public void setActive(boolean active) {
        this.active = active;
    }

    public LocalTime getTime() {
        return time;
    }

    public void setTime(LocalTime time) {
        this.time = time;
    }

    public Map<WeekDay, Boolean> getDays() {
        return days;
    }

    public void setDays(Map<WeekDay, Boolean> days) {
        this.days = days;
    }

    public int getValue() {
        return value;
    }

    public void setValue(int value) {
        this.value = value;
    }

    public static Timing[] getTimings() {
        return timings;
    }

    public static void setTimings(Timing[] timings) {
        Timing.timings = timings;
    }

    public String generateActiveDaysString() {
        String activeDays = "";

        if (days != null) {
            for (WeekDay weekDay : WeekDay.values()) {

                if (days.get(weekDay) == true) {
                    activeDays += " " + dayID.get(weekDay);
                }
            }
        }
        else {
            activeDays = "-";
        }

        return activeDays;
    }

    public JSONObject serializeTimingObject() {
        JSONObject timingObject = new JSONObject();

        try {
            //timingObject.put("ID", ID);
            timingObject.put("ACTIVE", active);

            float valueFloat = (value * 1.0f) / 100;
            timingObject.put("VALUE", valueFloat);

            String daysString = "";

            for (WeekDay weekDay : WeekDay.values()) {
                boolean opened = days.get(weekDay);
                if (opened) {
                    daysString += "T";
                }
                else {
                    daysString += "F";
                }
            }

            timingObject.put("DAYS", daysString);

            if (time == null) {
                timingObject.put("HOUR", -1);
                timingObject.put("MIN", -1);
            }
            else {
                timingObject.put("HOUR", time.getHourOfDay());
                timingObject.put("MIN", time.getMinuteOfHour());
            }

        } catch (JSONException e) {

        }

        return timingObject;
    }

    public static JSONObject serializeTimingObjectDump() {
        JSONObject timingConfig = new JSONObject();

        for(int i = 0; i < numberOfSlots; i++) {
            JSONObject timingObject = timings[i].serializeTimingObject();
            if (timingObject == null) {
                continue;
            }
            try {
                timingConfig.put(String.valueOf(i), timingObject);
            }
            catch (JSONException e) {

            }
        }

        return timingConfig;
    }

    public static Timing parseTimingObject(JSONObject timingObject) {
        int value = 100;
        String dayString = "FFFFFFF";

        Map<WeekDay, Boolean> dayMap = new HashMap<>();
        for (WeekDay day : WeekDay.values()) {
            dayMap.put(day, false);
        }

        LocalTime time = new LocalTime(12, 0);

        try {
            float floatValue = (float) timingObject.get("VALUE");
            value = (int) (floatValue * 100);

            String daysString = (String) timingObject.get("DAYS");
            for (int i = 0; i < daysString.length(); i++) {
                Boolean opened = false;
                if (daysString.charAt(i) == 'T') {
                    opened = true;
                }

                dayMap.put(WeekDay.values()[i], opened);
            }

            int hour = (int) timingObject.get("HOUR");
            int minute = (int) timingObject.get("MIN");

            if (hour == -1 || minute == -1) {
                time = null;
            }
            else {
                time = new LocalTime(hour, minute);
            }
        }
        catch (JSONException e) {
            e.printStackTrace();
        }

        return new Timing(value, dayMap, time);
    }

    public static void parseTimingObjectDump(String timingConfigString) {
        if (timingConfigString.length() > 0) {
            JSONObject timingConfig = null;
            try {
                timingConfig = new JSONObject(timingConfigString);

                for (Iterator<String> iter = timingConfig.keys(); iter.hasNext(); ) {
                    String key = iter.next();

                    JSONObject timingObject = (JSONObject) timingConfig.get(key);
                    Timing timing = parseTimingObject(timingObject);
                    timings[Integer.valueOf(key)] = timing;
                }

            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }

    public static void updateTimings(Activity activity) {
        String timingString = serializeTimingObjectDump().toString();
        SharedPreferences sharedPref = activity.getPreferences(Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString(activity.getResources().getString(R.string.timing_pref), timingString);
        editor.apply();
    }
}