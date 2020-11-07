#include <Arduino.h>

#include "dac_MCP4725/dac_MCP4725.h"
#include "debug/debug.h"

#include "viridian.h"

double viridian::chargingCurrent;

void viridian::initialize() {
    // set the charging current to 0
    viridian::stopCharging();

    // initialize the underlying dac
    dac_MCP4725::initialize();
}

void viridian::setChargingCurrent(const double maxAmps) {
    if (maxAmps > VIRIDIAN_MAX_RANGE_AMPS) {
        // if new value is more than max, apply max
        viridian::chargingCurrent = VIRIDIAN_MAX_RANGE_AMPS;
    } else if (maxAmps < VIRIDIAN_MIN_RANGE_AMPS) {
        // if value is less than minimum, just apply 0
        viridian::chargingCurrent = 0.0;
    } else {
        // this is an acceptable value
        viridian::chargingCurrent = maxAmps;
    }
}

void viridian::stopCharging() {
    viridian::setChargingCurrent(0.0);
}

double viridian::getChargingCurrent() {
    return viridian::chargingCurrent;
}

void viridian::sendToCar() {
    // special case to stop charging
    if (viridian::chargingCurrent == 0.0) {
        debug::log("Sending stop command to Viridian");

        // just send 0 to the DAC
        dac_MCP4725::write(0);
    } else {
        // IC equivalent voltage for the current value
        double ICV = VIRIDIAN_MIN_RANGE_ICV + (viridian::chargingCurrent - VIRIDIAN_MIN_RANGE_AMPS) * (VIRIDIAN_MAX_RANGE_ICV - VIRIDIAN_MIN_RANGE_ICV) / (VIRIDIAN_MAX_RANGE_AMPS - VIRIDIAN_MIN_RANGE_AMPS);

        // value for the DAC
        uint16_t dacValue = ICV * (VIRIDIAN_DAC_MAX_V - VIRIDIAN_DAC_MIN_V) / VIRIDIAN_DAC_MAX_Q;

        debug::log("Sending charging command to Viridian at "+String(viridian::chargingCurrent)+"A, IC equivalent voltage: "+String(ICV)+"V, DAC Value: "+String(dacValue));

        // Send the value to the DAC
        dac_MCP4725::write(dacValue);        
    }
}