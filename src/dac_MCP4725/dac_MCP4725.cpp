#include <Arduino.h>
#include <Wire.h>

#include "debug/debug.h"

#include "dac_MCP4725.h"

void dac_MCP4725::initialize() {
    // initialze the wire interface
    Wire.begin();
}

void dac_MCP4725::write(int value) {
    // begin the transmission to the DAC
    Wire.beginTransmission(DAC_MCP4725_I2C_ADDRESS);

    // send the first command word: standard speed write with no EEPROM
    Wire.write(0b01000000);

    // send the first part of the value
    Wire.write((value & 0xFF0) >> 4);

    // send the last part of the value
    Wire.write((value & 0xF) << 4);

    // end the transmission
    byte endTransmission = Wire.endTransmission(); 
    if (endTransmission!= 0) {
        debug::log("dac_MCP4725: Error during I2C transmission - "+String(endTransmission));
    }
}