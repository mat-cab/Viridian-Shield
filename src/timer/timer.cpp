#include <Arduino.h>

#include "timer.h"

uint32_t timer::lastOccurence;
uint32_t timer::duration;

void timer::setTimerDuration(const uint32_t durationMS) {
    timer::duration = durationMS;
    // also reset the timer
    timer::resetTimer();
}

void timer::resetTimer() {
    timer::lastOccurence = millis();
}

boolean timer::timerAllows() {
    // check if last occurence was more than duration ago
    if (millis() - timer::lastOccurence >= timer::duration) {
        // if so reset the timer
        timer::resetTimer();
        // and return true
        return true;
    } else {
        // otherwise do not allow
        return false;
    }
}