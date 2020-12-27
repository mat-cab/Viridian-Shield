#pragma once

#include <Arduino.h>

// Association between options and pins
// pin 8 is to add 2A to the current margin
static const uint8_t INPUTS_OPTION_MARGIN_ADD_2A = 8;
// pin 9 is to add 1A to the current margin
static const uint8_t INPUTS_OPTION_MARGIN_ADD_1A = 9;
// pin 7 is the signal when the car is charging
static const uint8_t INPUTS_OPTION_CHARGING_CAR = 7;

// Association between variables and pins

class inputs {
    public:
        static void initialize();
        static bool readOption(uint8_t pin);
        static int readVariable(uint8_t pin);
};