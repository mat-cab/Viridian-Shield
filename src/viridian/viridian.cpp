#include <Arduino.h>

#include "dac_MCP4725/dac_MCP4725.h"
#include "debug/debug.h"

#include "viridian.h"

double viridian::_chargingCurrent;
boolean viridian::_currentChanged;

void viridian::initialize() {
    // initialize the underlying dac
    dac_MCP4725::initialize();

    // set the starting charging current to not 0
    viridian::_chargingCurrent = 1.0;

    // set the charging current to 0
    viridian::stopCharging();
}

void viridian::setChargingCurrent(const double maxAmps) {
    double newChargingCurrent;

    if (maxAmps > VIRIDIAN_MAX_RANGE_AMPS) {
        // if new value is more than max, apply max
        newChargingCurrent = VIRIDIAN_MAX_RANGE_AMPS;
    } else if (maxAmps < VIRIDIAN_MIN_RANGE_AMPS) {
        // if value is less than minimum, just apply 0
        newChargingCurrent = 0.0;
        // also log a message
        debug::logNoLine(F("viridian: Not charging - new charging current is less than minimum of "));
        debug::logNoLine(String(VIRIDIAN_MIN_RANGE_AMPS));
        debug::log(F(" Amps"));
    } else {
        // this is an acceptable value
        newChargingCurrent = maxAmps;
    }

    // if this is a new command
    if (newChargingCurrent != viridian::_chargingCurrent) {
        // save it
        viridian::_chargingCurrent = newChargingCurrent;

        // also send directly the command to the car
        viridian::sendToCar();

        // also state that the current has changed
        viridian::_currentChanged = true;
    }
}

void viridian::stopCharging() {
    viridian::setChargingCurrent(0.0);
}

double viridian::getChargingCurrent() {
    return viridian::_chargingCurrent;
}

void viridian::resetChange() {
    viridian::_currentChanged = false;
}

boolean viridian::currentChanged() {
    return viridian::_currentChanged;
}

void viridian::sendToCar() {
    // special case to stop charging
    if (viridian::_chargingCurrent == 0.0) {
        debug::log(F("viridian: Sending stop command to Viridian"));

        // just send 0 to the DAC
        dac_MCP4725::write(0);
    } else {
        // Correct the charging current (because of offset)
        double correctedChargingCurrent = viridian::_chargingCurrent + VIRIDIAN_MEASURED_OFFSET;

        // IC equivalent voltage for the current value
        double ICV = VIRIDIAN_MIN_RANGE_ICV + (correctedChargingCurrent - VIRIDIAN_MIN_RANGE_AMPS) * (VIRIDIAN_MAX_RANGE_ICV - VIRIDIAN_MIN_RANGE_ICV) / (VIRIDIAN_MAX_RANGE_AMPS - VIRIDIAN_MIN_RANGE_AMPS);

        // value for the DAC
        uint16_t dacValue = (ICV - VIRIDIAN_DAC_MIN_V) / (VIRIDIAN_DAC_MAX_V - VIRIDIAN_DAC_MIN_V) * VIRIDIAN_DAC_MAX_Q;

        debug::logNoLine(F("viridian: Sending charging command to Viridian at "));
        debug::logNoLine(String(viridian::_chargingCurrent));
        debug::logNoLine(F("A, IC equivalent voltage: "));
        debug::logNoLine(String(ICV));
        debug::logNoLine(F("V, DAC Value: "));
        debug::log(String(dacValue));

        // Send the value to the DAC
        dac_MCP4725::write(dacValue);        
    }
}