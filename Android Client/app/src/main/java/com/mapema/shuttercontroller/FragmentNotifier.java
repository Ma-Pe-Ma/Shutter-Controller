package com.mapema.shuttercontroller;

public interface FragmentNotifier {
    void notifySelectorAboutNewTiming(int position, Timing timing);
    void notifySelectorAboutCanceling(int position);
}
