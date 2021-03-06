#pragma once

#include <Arduino.h>

// working range : 6-32A
static const double VIRIDIAN_MIN_RANGE_AMPS = 6.0;
static const double VIRIDIAN_MAX_RANGE_AMPS = 32.0;

// IC equivalent voltage for min and max values
static const double VIRIDIAN_MIN_RANGE_ICV = 0.8018;
static const double VIRIDIAN_MAX_RANGE_ICV = 2.1132;

// DAC min and max output voltage
static const double VIRIDIAN_DAC_MIN_V = 0.0;
static const double VIRIDIAN_DAC_MAX_V = 5.0;

// DAC max value
static const uint16_t VIRIDIAN_DAC_MAX_Q = 4095;

// Measured offset for current measurement
// positive offset means that the real current is too low
// negative offset means that the real current is too high
static const double VIRIDIAN_MEASURED_OFFSET = -2;

class viridian {
    public:
        static void initialize();

        static void setChargingCurrent(const double maxAmps);
        static void stopCharging();
        static double getChargingCurrent();
        
        static void resetChange();
        static boolean currentChanged();

    private:
        static void sendToCar();

        static double _chargingCurrent;
        static boolean _currentChanged;
};