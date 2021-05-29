#include <Arduino.h>

#include "debug.h"

void debug::initialize() {
    // Open Serial
    Serial.begin(DEBUG_SERIAL_SPEED);
    // Wait for serial to be ready
    while (!Serial) ;
}

void debug::log(const String message) {
    Serial.println(message);
}

void debug::logNoLine(const String message) {
    Serial.print(message);
}