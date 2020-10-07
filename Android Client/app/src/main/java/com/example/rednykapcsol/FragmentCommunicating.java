package com.example.rednykapcsol;

public interface FragmentCommunicating {

    void notifySelectorAboutNewTiming(int position, Timing timing);
    void notifySelectorAboutCanceling(int position);
    void notifyConfig();
    void notifyMessage(String[] newMessages);
    void notifyAboutTimedUpdate(int secondsTillUpdate);
}
