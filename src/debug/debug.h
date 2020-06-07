#pragma once

#include <Arduino.h>

class debug {
    public:
        debug();
        void log(const char* message);
};