#pragma once

#include <Arduino.h>

// Association between options and pins
#define INPUTS_OPTION_16A_MAX 7

// Association between variables and pins
#define INPUTS_VARIABLE_MAX_CURRENT 23

class inputs {
    public:
        static void initialize();
        static bool readOption(uint8_t pin);
        static int readVariable(uint8_t pin);
};