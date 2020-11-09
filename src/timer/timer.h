#pragma once

#include <Arduino.h>

class timer {
    public:
        static void setTimerDuration(const uint32_t durationMS);
        static boolean timerAllows();
        static void resetTimer();
    private:
        static uint32_t lastOccurence;
        static uint32_t duration;
};