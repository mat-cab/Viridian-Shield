#pragma once

#include <Arduino.h>

// I2C address of the DAC
static const uint8_t DAC_MCP4725_I2C_ADDRESS = 0b1100000;

class dac_MCP4725 {
    public:
        static void initialize();
        static void write(int value);
};