#include <Arduino.h>

#include "inputs.h"

void inputs::initialize() {
    // set pins 2 to 8 in INPUT PULLUP mode
    for (uint8_t i = 2; i < 8; i++) {
        pinMode(i, INPUT_PULLUP);
    }

    // set digital/analog pins to INPUT
    pinMode(16, INPUT_PULLUP);
    pinMode(17, INPUT_PULLUP);

    // set analog pins to INPUT
    pinMode(14, INPUT);
    pinMode(15, INPUT);
}

bool inputs::readOption(uint8_t pin) {
    // return digitalread, but inverse because of PULLUP mode
    return digitalRead(pin)==LOW;
}

int inputs::readVariable(uint8_t pin) {
    // return the value for the selected pin
    return analogRead(pin);
}