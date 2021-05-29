#include <Arduino.h>

#include "debug.h"

void debug::initialize() {
    // debug generally only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
    // Set pins for FTD 1232 on pins 14 to 18:
    // DTR is not on an Arduino pin
    // pin 14 is RX (on FTD)
    // pin 15 is TX (on FTD)
    // pin 16 is VCC
    pinMode(16, OUTPUT);
    digitalWrite(16, HIGH);
    // pin 17 is CTS
    pinMode(17, INPUT);
    // pin 18 is GND
    pinMode(18, OUTPUT);
    pinMode(18, LOW);
    // Open Serial
    Serial3.begin(DEBUG_SERIAL_SPEED);
    // Wait for serial to be ready
    while (!Serial3) ;
    #endif
}

void debug::log(const String message) {
    // debug generally only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
    Serial3.println(message);
    #endif
}

void debug::logNoLine(const String message) {
    // debug generally only available for MEGA on Serial1
    #ifdef ARDUINO_AVR_MEGA2560
    Serial3.print(message);
    #endif
}