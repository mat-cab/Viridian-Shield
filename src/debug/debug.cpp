#include <Arduino.h>

#include "debug.h"

debug::debug() {
    // debug only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
        if (!Serial1) {
            Serial1.begin(9600);
        }
    #endif
}

void debug::log(const char* message) {
    // debug only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
        Serial1.write(message);
    #endif
}