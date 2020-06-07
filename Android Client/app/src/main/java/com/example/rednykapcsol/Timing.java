package com.example.rednykapcsol;

import android.content.Context;
import android.util.Log;
import android.util.Pair;

import org.joda.time.LocalTime;

import java.time.DayOfWeek;
import java.util.HashMap;
import java.util.Map;
import com.example.rednykapcsol.WeekDay;

public class Timing {
    private static final int numberOfSlots = 12;
    private static Timing timings[] = new Timing[numberOfSlots];
    private static Map<WeekDay, String> dayID;

    private boolean active;
    private int value = 100;
    private LocalTime time;
    private Map<WeekDay, Boolean> days;

    public static void initialize() {
        dayID = new HashMap<>();

        dayID.put(WeekDay.MONDAY, "H");
        dayID.put(WeekDay.TUESDAY, "K");
        dayID.put(WeekDay.WEDNESDAY, "Sze");
        dayID.put(WeekDay.THURSDAY, "Cs");
        dayID.put(WeekDay.FRIDAY, "P");
        dayID.put(WeekDay.SATURDAY, "Szo");
        dayID.put(WeekDay.SUNDAY, "V");
    }

    public Timing() {
        time = null;
        days = null;
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

    public String generateDaysString() {
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

    public static String generateTimingString() {
        String result = "";

        for(int i = 0; i < numberOfSlots; i++) {
            Timing timing = timings[i];

            if(timing.getDays() != null) {
                result += i + ";";

                if (timing.isActive()) {
                    result += "T;";
                }
                else {
                    result += "F;";
                }

                result += timing.getTime().getHourOfDay()+":"+timing.getTime().getMinuteOfHour()+";";

                for (WeekDay weekDay : WeekDay.values()) {
                    boolean active = timing.getDays().get(weekDay);

                    if(active) {
                        result += "T";
                    }
                    else {
                        result += "F";
                    }
                }
                result += ";"+timing.getValue()+'\n';
            }
            else {
                result += "" + i +'\n';
            }
        }

        //SendToServer();

        Log.i("DEBUG","Saved string: "+result);
        return result;
    }

    public static void parsePreferences(String preferences) {

        if(preferences.length() < 10) {
            for (int i = 0; i < numberOfSlots; i++) {
                timings[i] = new Timing();
            }
        }
        else {
            String[] timingStrings = preferences.split("\n");

            for(int i = 0; i < numberOfSlots; i++) {
                if (timingStrings[i].length()<=2) {
                    Timing timing = new Timing();
                    timings[i] = timing;
                }
                else {
                    String[] properties = timingStrings[i].split(";");

                    boolean switched = false;

                    if (properties[1].charAt(0) =='F') {
                        switched = false;
                    }
                    if (properties[1].charAt(0) =='T') {
                        switched = true;
                    }

                    String[] timeString = properties[2].split(":");
                    LocalTime time = new LocalTime(Integer.parseInt(timeString[0]), Integer.parseInt(timeString[1]));
                    int value = Integer.parseInt(properties[4]);

                    Map<WeekDay, Boolean> days = new HashMap<>();
                    for(int j = 0; j < 7; j++) {
                        boolean state = false;

                        if (properties[3].charAt(j) =='F') {
                            state = false;
                        }
                        if (properties[3].charAt(j) =='T') {
                            state = true;
                        }

                        WeekDay weekDay = WeekDay.values()[j];
                        days.put(weekDay, state);
                    }

                    timings[i] = new Timing(value, days, time);
                    timings[i].setActive(switched);
                }
            }
        }
    }
}