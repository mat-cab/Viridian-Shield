#include <Arduino.h>

#include "inputs.h"

void inputs::initialize() {
    // set pins 2 to 9 in INPUT PULLUP mode
    for (uint8_t i = 2; i < 10; i++) {
        pinMode(i, INPUT_PULLUP);
    }
}

bool inputs::read(uint8_t pin) {
    // return not digitalread (because of pullup mode)
    return !digitalRead(pin);
}