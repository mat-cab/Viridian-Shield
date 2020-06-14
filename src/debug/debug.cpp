#include <Arduino.h>

#include "debug.h"

void debug::initialize() {
    // debug generally only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
    if (!Serial1) {
        Serial1.begin(DEBUG_SERIAL_SPEED);
    }
    #endif

    // possibility to force debug output to Serial on UNO
    // Note that it will conflict with Teleinfo in (on RX)
    // Therefore switch RX_IN must be OFF to properly use this debug on Uno
    #ifdef ARDUINO_AVR_UNO
    if (DEBUG_FORCE && !Serial) {
        Serial.begin(DEBUG_SERIAL_SPEED);
    }
    #endif
}

void debug::log(const char* message) {
    // debug generally only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
        Serial1.write(message);
    #endif

    // debug on Uno if forced
    #ifdef ARDUINO_AVR_UNO
    if (DEBUG_FORCE) {
        Serial.write(message);
    }
    #endif
}