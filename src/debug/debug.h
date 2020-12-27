#pragma once

#include <Arduino.h>

// Standard Serial Speed
static const int DEBUG_SERIAL_SPEED = 9600;

class debug {
    public:
        static void initialize();
        static void log(const String message);
};