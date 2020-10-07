package com.example.rednykapcsol;

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

    public MessageContainer() {
        for (String message : messages) {
            message = "";
        }
    }

    public void addNewMessages(String[] newMessages) {
        if (newMessages.length >= 10) {
            for (int i = 0; i < numberOfMessages; i++) {
                messages[i] = newMessages[i];
            }
            return;
        }

        int length = newMessages.length;

        for (int i = 0; i < numberOfMessages - length; i++) {
            messages[numberOfMessages - 1 - i] = messages[numberOfMessages - 1 - length - i];
        }

        System.arraycopy(newMessages, 0, messages, 0, length);
    }

    public String[] getMessages() {
        return messages;
    }

    public static String[] processMessages(String[] messages) {

        return null;
    }
}
