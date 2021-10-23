package com.example.rednykapcsol;

import android.content.Context;
import android.util.Log;

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

    public final int numberOfMessages = 10;
    private Message[] messages = new Message[numberOfMessages];

    private String startupTime;

    public MessageContainer() {
        for (int i = 0; i < numberOfMessages; i++) {
            messages[i] = new Message("", "", "");
        }
    }

    public Message getMessage(int ID) {
        return messages[ID];
    }


    public void updateMessages(JSONObject messagesObject) {
        for (int i = 0; i < numberOfMessages; i++) {
            try {
                JSONObject value = (JSONObject) messagesObject.get(String.valueOf(i));
                Message newMessage = processJSONMessage(i, value);
                messages[i] = newMessage;
            } catch (JSONException e) {
                e.printStackTrace();
            }

        }
    }

    public void addNewMessages(List<JSONObject> newMessages) {
        if (newMessages.size() >= numberOfMessages) {
            for (int i = 0; i < numberOfMessages; i++) {
                //messages[i] = processJSONMessage(newMessages.get(i));
            }
            return;
        }

        int length = newMessages.size();

        for (int i = 0; i < numberOfMessages - length; i++) {
            messages[numberOfMessages - 1 - i] = messages[numberOfMessages - 1 - length - i];
        }

        for (int i = 0; i < length; i++) {
            //messages[i] = processJSONMessage(newMessages.get(i));
        }
    }

    public List<String> getFilteredMessages(Context context, int size) {
        if (size > numberOfMessages || size == 0) {
            size = numberOfMessages;
        }

        List<String> filteredMessages = new ArrayList<>();

        for(int i = 0; i < size; i++) {
            if (!messages[i].equals("")) {
                //filteredMessages.add(messages[i]);
            }
        }

        if (filteredMessages.size() == 0) {
            String noMessage = context.getString(R.string.noMessage);
            filteredMessages.add(noMessage);
        }

        return filteredMessages;
    }

    public static Message processJSONMessage(int ID, JSONObject message) {
        String date = "E";
        String event = "E";
        String idText = "E";
        try {
            idText = String.valueOf(ID + 1) + ".";
            date = (String) message.get("D");
            String T = (String) message.get("T");
            String R = (String) message.get("R");
            String A = (String) message.get("A");

            event = createMessage(T, R, A);

            if (T.compareTo("E") == 0) {
                idText = "";
                date = "";
                event = "";
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

        return new Message(idText ,event, date);
    }

    private static String createMessage(String type, String res, String add) {
        if (type.compareTo("E") == 0) {
            return AppContext.getAppContext().getString(R.string.hyphen);
        }

        if (type.compareTo("I") == 0) {
            return AppContext.getAppContext().getString(R.string.serverStart);
        }

        if (type.compareTo("J") == 0) {
            return  AppContext.getAppContext().getString(R.string.jsonError, res, add);
        }

        if (type.compareTo("Z") == 0) {
            if (res.compareTo("F") == 0) {
                return AppContext.getAppContext().getString(R.string.nullError);
            }
            if (res.compareTo("O") == 0) {
                if (add.compareTo("U") == 0) {
                    return AppContext.getAppContext().getString(R.string.nullSuccessUp);
                }
                else if (add.compareTo("D") == 0) {
                    return AppContext.getAppContext().getString(R.string.nullSuccessDown);
                }
            }
        }

        if (type.compareTo("T") == 0) {
            return AppContext.getAppContext().getString(R.string.scheduleSuccess, res, add);
        }

        if (type.compareTo("S") == 0) {
            if (res.compareTo("M") == 0) {
                return AppContext.getAppContext().getString(R.string.manualSet, add);
            }

            if (res.compareTo("Z") == 0) {
                return AppContext.getAppContext().getString(R.string.positionFound, add);
            }

            if (res.compareTo("T") == 0) {
                return AppContext.getAppContext().getString(R.string.scheduleUpdated);
            }
        }

        return AppContext.getAppContext().getString(R.string.unknownEvent, type, res, add);
    }

    public String getStartupTime() {
        return startupTime;
    }

    public void setStartupTime(String startupTime) {
        this.startupTime = startupTime;
    }
}
