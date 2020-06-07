#include <Arduino.h>

#include "debug.h"

#ifdef ARDUINO_AVR_UNO
// Option to force the debug on Serial for the Arduino UNO
static const boolean FORCE_DEBUG = false;
#endif

// Standard Serial Speed
static const int SERIAL_SPEED = 9600;

debug::debug() {
    // debug generally only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
    if (!Serial1) {
        Serial1.begin(SERIAL_SPEED);
    }
    #endif

    // possibility to force debug output to Serial on UNO
    // Note that it will conflict with Teleinfo in (on RX)
    // Therefore switch RX_IN must be OFF to properly use this debug on Uno
    #ifdef ARDUINO_AVR_UNO
    if (FORCE_DEBUG && !Serial) {
        Serial.begin(SERIAL_SPEED);
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
    if (FORCE_DEBUG) {
        Serial.write(message);
    }
    #endif
}