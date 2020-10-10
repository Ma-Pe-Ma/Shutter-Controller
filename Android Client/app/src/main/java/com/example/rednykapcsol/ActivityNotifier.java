package com.example.rednykapcsol;

public interface ActivityNotifier {
    void notifyAboutTimedUpdate(int secondsTillUpdate);
    void notifyMessage(int size);
    void notifyNewValue(float newValue);
    void notifyTimeout();
    void notifyEnableGUI();
    void notifyDisableGUI();
}
