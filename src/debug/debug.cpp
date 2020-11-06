#include <Arduino.h>

#include "debug.h"

void debug::initialize() {
    // debug generally only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
    // Set pins for FTD 1232 on pins 15 to 20:
    // pin 15 is DTR
    pinMode(15, INPUT);
    // pin 16 is RX (on FTD)
    // pin 17 is TX (on FTD)
    // pin 18 is VCC
    pinMode(18, OUTPUT);
    digitalWrite(18, HIGH);
    // pin 19 is CTS
    pinMode(19, INPUT);
    // pin 20 is GND
    pinMode(20, OUTPUT);
    pinMode(20, LOW);
    // Open Serial
    Serial2.begin(DEBUG_SERIAL_SPEED);
    // Wait for serial to be ready
    while (!Serial2) ;
    #endif

    // possibility to force debug output to Serial on UNO
    // Note that it will conflict with Teleinfo in (on RX)
    // Therefore switch RX_IN must be OFF to properly use this debug on Uno
    #ifdef ARDUINO_AVR_UNO
    if (DEBUG_FORCE) {
        // Open Serial
        Serial.begin(DEBUG_SERIAL_SPEED);
        // Wait for serial to be ready
        while (!Serial) ;
    }
    #endif
}

void debug::log(const String message) {
    // debug generally only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
    Serial2.println(message);
    #endif

    // debug on Uno if forced
    #ifdef ARDUINO_AVR_UNO
    if (DEBUG_FORCE) {
        Serial.println(message);
    }
    #endif
}