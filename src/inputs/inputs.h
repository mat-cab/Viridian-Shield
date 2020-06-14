#pragma once

#include <Arduino.h>

// Association between functions and pins
#define INPUTS_16A_MAX 7

class inputs {
    public:
        static void initialize();
        static bool read(uint8_t pin);
};