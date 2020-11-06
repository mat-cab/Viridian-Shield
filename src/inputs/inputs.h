#pragma once

#include <Arduino.h>

// Association between options and pins
// pin 8 is to add 2A to the current margin
static const uint8_t INPUTS_OPTION_MARGIN_ADD_2A = 8;

// Association between variables and pins

class inputs {
    public:
        static void initialize();
        static bool readOption(uint8_t pin);
        static int readVariable(uint8_t pin);
};