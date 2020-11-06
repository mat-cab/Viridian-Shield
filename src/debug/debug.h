#pragma once

#include <Arduino.h>

#ifdef ARDUINO_AVR_UNO
// Option to force the debug on Serial for the Arduino UNO
static const boolean DEBUG_FORCE = true;
#endif

// Standard Serial Speed
static const int DEBUG_SERIAL_SPEED = 9600;

class debug {
    public:
        static void initialize();
        static void log(const String message);
};