package com.example.rednykapcsol;

public class Message {
    private String ID;
    private String event;
    private String date;

    public Message(String ID, String event, String date) {
        this.ID = ID;
        this.event = event;
        this.date = date;
    }

    public String getID() {
        return ID;
    }

    public void setID(String ID) {
        this.ID = ID;
    }

    public String getEvent() {
        return event;
    }

    public void setEvent(String event) {
        this.event = event;
    }

    public String getDate() {
        return date;
    }

    public void setDate(String date) {
        this.date = date;
    }
}
