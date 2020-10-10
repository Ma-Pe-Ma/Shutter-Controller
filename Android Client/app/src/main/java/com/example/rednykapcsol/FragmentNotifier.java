package com.example.rednykapcsol;

public interface FragmentNotifier {
    void notifySelectorAboutNewTiming(int position, Timing timing);
    void notifySelectorAboutCanceling(int position);
}
