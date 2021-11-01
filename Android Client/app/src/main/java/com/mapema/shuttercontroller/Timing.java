package com.mapema.shuttercontroller;

import android.app.Activity;
import android.util.Log;

import org.joda.time.LocalTime;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class Timing {
    public static final int numberOfSlots = 6;
    private static Timing[] timings = new Timing[numberOfSlots];
    private static Timing[] bufferTimings = new Timing[numberOfSlots];
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
            bufferTimings[i] = new Timing();
        }
    }

    public Timing() {
        time = null;
        days = new HashMap<>();

        for (WeekDay day : WeekDay.values()) {
            days.put(day, false);
        }

        value = 100;
    }

    public Timing(int value, Map<WeekDay, Boolean> days, LocalTime time, Boolean active) {
        this.value = value;
        this.days = days;
        this.time = time;
        this.active = active;
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

    public static Timing[] getBufferTimings() {
        return bufferTimings;
    }

    public static void setTimings(Timing[] timings) {
        Timing.timings = timings;
    }

    public static void updateBuffer() {
        for (int i = 0; i < numberOfSlots; i++) {
            bufferTimings[i].value = timings[i].value;
            bufferTimings[i].days = timings[i].days;
            bufferTimings[i].time = timings[i].time;
            bufferTimings[i].active = timings[i].active;
        }
    }

    public static void updateTimings() {
        for (int i = 0; i < numberOfSlots; i++) {
            timings[i].value = bufferTimings[i].value;
            timings[i].days = bufferTimings[i].days;
            timings[i].time = bufferTimings[i].time;
            timings[i].active = bufferTimings[i].active;
        }
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
            timingObject.put("A", active);
            timingObject.put("V", value);

            String daysString = "";

            for (WeekDay weekDay : WeekDay.values()) {
                boolean opened = days.get(weekDay);
                if (opened) {
                    daysString += 'T';
                }
                else {
                    daysString += 'F';
                }
            }

            timingObject.put("D", daysString);

            if (time == null) {
                timingObject.put("H", -1);
                timingObject.put("M", -1);
            }
            else {
                timingObject.put("H", time.getHourOfDay());
                timingObject.put("M", time.getMinuteOfHour());
            }

        } catch (JSONException e) {

        }

        return timingObject;
    }

    public static JSONObject generateTimingObjectDump() {
        JSONObject timingConfig = new JSONObject();

        for(int i = 0; i < numberOfSlots; i++) {
            JSONObject timingObject = bufferTimings[i].serializeTimingObject();
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
        boolean active = false;

        Map<WeekDay, Boolean> dayMap = new HashMap<>();
        for (WeekDay day : WeekDay.values()) {
            dayMap.put(day, false);
        }

        LocalTime time = new LocalTime(12, 0);

        try {
            Log.i("DEBUG", "Timing created!");

            value  = timingObject.getInt("V");

            String daysString = (String) timingObject.get("D");
            for (int i = 0; i < daysString.length(); i++) {
                Boolean opened = false;
                if (daysString.charAt(i) == 'T') {
                    opened = true;
                }

                dayMap.put(WeekDay.values()[i], opened);
            }

            active = (boolean) timingObject.get("A");

            int hour = (int) timingObject.get("H");
            int minute = (int) timingObject.get("M");

            if (hour == -1 || minute == -1) {
                time = null;
            }
            else {
                time = new LocalTime(hour, minute);
            }
        }
        catch (JSONException e) {

        }

        return new Timing(value, dayMap, time, active);
    }

    public static void parseTimingObjectDump(JSONObject  timingConfig) {
        try {
            for (Iterator<String> iter = timingConfig.keys(); iter.hasNext(); ) {
                String key = iter.next();

                JSONObject timingObject = (JSONObject) timingConfig.get(key);
                Timing timing = parseTimingObject(timingObject);
                timings[Integer.parseInt(key)] = timing;
            }

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }
}