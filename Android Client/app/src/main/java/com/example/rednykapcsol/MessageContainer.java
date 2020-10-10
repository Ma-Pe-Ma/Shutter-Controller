package com.example.rednykapcsol;

import android.content.Context;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MessageContainer {
    private static MessageContainer messageContainer;

    public static MessageContainer getMessageContainer() {
        if (messageContainer == null) {
            messageContainer = new MessageContainer();
        }

        return messageContainer;
    }

    private final int numberOfMessages = 10;
    private String[] messages = new String[numberOfMessages];

    private String startupTime;

    public MessageContainer() {
        Arrays.fill(messages, "");
    }

    public void addNewMessages(List<String> newMessages) {
        if (newMessages.size() >= numberOfMessages) {
            for (int i = 0; i < numberOfMessages; i++) {
                messages[i] = processJSONMessage(newMessages.get(i));
            }
            return;
        }

        int length = newMessages.size();

        for (int i = 0; i < numberOfMessages - length; i++) {
            messages[numberOfMessages - 1 - i] = messages[numberOfMessages - 1 - length - i];
        }

        for (int i = 0; i < length; i++) {
            messages[i] = processJSONMessage(newMessages.get(i));
        }
    }

    public List<String> getFilteredMessages(Context context, int size) {
        if (size > numberOfMessages || size == 0) {
            size = numberOfMessages;
        }

        List<String> filteredMessages = new ArrayList<>();

        for(int i = 0; i < size; i++) {
            if (!messages[i].equals("")) {
                filteredMessages.add(messages[i]);
            }
        }

        if (filteredMessages.size() == 0) {
            String noMessage = context.getString(R.string.noMessage);
            filteredMessages.add(noMessage);
        }

        return filteredMessages;
    }

    //TODO: Implement this
    public static String processJSONMessage(String message) {
        try {
            JSONObject messageObject = new JSONObject(message);
        }
        catch (JSONException e) {

        }

        return message;
    }

    public String getStartupTime() {
        return startupTime;
    }

    public void setStartupTime(String startupTime) {
        this.startupTime = startupTime;
    }
}
