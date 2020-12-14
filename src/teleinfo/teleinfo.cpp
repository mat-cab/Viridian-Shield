#include <Arduino.h>

#include "debug/debug.h"

#include "teleinfo.h"

char teleinfo::labelBuffer[TELEINFO_LABEL_BUFFER_SIZE];
char teleinfo::valueBuffer[TELEINFO_VALUE_BUFFER_SIZE];

void teleinfo::initialize() {
    // clear the buffer
    teleinfo::clearBuffer();

    // start Serial
    Serial.begin(1200, SERIAL_7E1);

    // wait for Serial to become available
    while (!Serial) ;
}

teleinfo_t teleinfo::read() {
    // structure for the return
    teleinfo_t result;

    // Begin by flushing the serial interface
    while (Serial.available())
        Serial.read();
    debug::log("teleinfo: Serial flushed");

    // Find the end of the ongoing teleinfo frame
    do {            
        teleinfo::readLine();
    } while (!strcmp(teleinfo::labelBuffer, "MOTDETAT"));
    debug::log("teleinfo: found end of teleinfo frame");

    // we are at the start of a frame
    do {
        if (teleinfo::readLine()) {                
            // try to match the label with all available labels
            if (record("ADCO", result.ADCO)) continue;
            if (record("OPTARIF", result.OPTARIF)) continue;
            if (record("ISOUSC", result.ISOUSC)) continue;
            if (record("IINST", result.IINST)) continue;
            if (record("IINST1", result.IINST1)) continue;
            if (record("IINST2", result.IINST2)) continue;
            if (record("IINST3", result.IINST3)) continue;
            if (record("ADPS", result.ADPS)) continue;
            if (record("IMAX", result.IMAX)) continue;
            if (record("IMAX1", result.IMAX1)) continue;
            if (record("IMAX2", result.IMAX2)) continue;
            if (record("IMAX3", result.IMAX3)) continue;
            if (record("PAPP", result.PAPP)) continue;
            if (record("PMAX", result.PMAX)) continue;
            if (record("BASE", result.BASE)) continue;
            if (record("HC_HC", result.HC_HC)) continue;
            if (record("HC_HP", result.HC_HP)) continue;
            if (record("EJP_HN", result.EJP_HN)) continue;
            if (record("EJP_HPM", result.EJP_HPM)) continue;
            if (record("PEJP", result.PEJP)) continue;
            if (record("BBR_HC_JB", result.BBR_HC_JB)) continue;
            if (record("BBR_HP_JB", result.BBR_HP_JB)) continue;
            if (record("BBR_HC_JW", result.BBR_HC_JW)) continue;
            if (record("BBR_HP_JW", result.BBR_HP_JW)) continue;
            if (record("BBR_HC_JR", result.BBR_HC_JR)) continue;
            if (record("BBR_HP_JR", result.BBR_HP_JR)) continue;
            if (record("DEMAIN", result.DEMAIN)) continue;
            if (record("HHPHC", &result.HHPHC)) continue;
            if (record("MOTDETAT", result.MOTDETAT)) continue;

            // no match: probably an unknow label ?
            debug::log("teleinfo: Unknown label found - label: "+String(teleinfo::labelBuffer)+" - value: "+String(teleinfo::valueBuffer));
        } else {
            debug::log("teleinfo: read failed");
        }
    } while (!strcmp(teleinfo::labelBuffer, "MOTDETAT"));

    // return the result (at the end)
    return result;
}

void teleinfo::clearBuffer() {
    memset( teleinfo::labelBuffer, '\0', TELEINFO_LABEL_BUFFER_SIZE);
    memset( teleinfo::valueBuffer, '\0', TELEINFO_VALUE_BUFFER_SIZE);
}

bool teleinfo::readLine() {
    uint8_t cks = 32;
    uint8_t messageCks;

    // clear the buffer
    teleinfo::clearBuffer();

    // First read the label
    if (teleinfo::readWord(teleinfo::labelBuffer, TELEINFO_LABEL_BUFFER_SIZE, cks) ) {
        // Next read the value
        if (teleinfo::readWord(teleinfo::valueBuffer, TELEINFO_VALUE_BUFFER_SIZE, cks)) {
            // Read the final cks
            while (!Serial.available()) ;
            messageCks = Serial.read();

            // Read the end of line
            while (!Serial.available()) ;
            if (Serial.read() != '\n') {
                // if not EOL, probably an issue somewhere else ?
                debug::log("teleinfo: EOL was not found when expected");

                return false;
            }

            // compute our own cks
            cks = (cks & 0x3F) + 0x20;

            // return whether the cks are identical
            return (cks==messageCks);
        } else {
            // log an issue
            debug::log("teleinfo: error while reading a value");
        }
    } else {
        // log an issue
        debug::log("teleinfo: error while reading a label");
    }

    // return there was an issue
    return false;
}

inline bool teleinfo::readWord(char* buffer, uint8_t maxBufferLength, uint8_t &cks) {
    uint8_t counter = 0;
    char c;

    // Read one word
    do {
        // wait for a char to be available
        while (!Serial.available()) ;

        // read one char
        c = Serial.read();

        // compute cks
        cks += (uint8_t)c;

        // assign to buffer (if not empty)
        if (c != ' ')  {
            buffer[counter++] = c;
        }
    } while (c != ' ' && counter < maxBufferLength);

    if (counter == maxBufferLength) {
        // check for buffer overflow
        debug::log("teleinfo: buffer overflow !!! buffer cleared");
        debug::log("teleinfo: buffer was "+String(buffer));

        // clear the buffer
        teleinfo::clearBuffer();
    }

    // return whether the buffer overflowed or not
    return (counter != maxBufferLength);
}

bool teleinfo::record(const char* label, char* destination) {
    // compare the label with the expected label
    if (strcmp(teleinfo::labelBuffer, label)) {
        // copy the value to the destination
        strcpy(destination, teleinfo::valueBuffer);
        // return OK
        return true;
    }

    return false;
}

bool teleinfo::record(const char* label, uint8_t &destination) {
    // compare the label with the expected label
    if (strcmp(teleinfo::labelBuffer, label)) {
        // convert the value to the destination
        destination = atoi(teleinfo::valueBuffer);
        // return OK
        return true;
    }

    return false;
}

bool teleinfo::record(const char* label, uint32_t &destination) {
    // compare the label with the expected label
    if (strcmp(teleinfo::labelBuffer, label)) {
        // convert the value to the destination
        destination = atol(teleinfo::valueBuffer);
        // return OK
        return true;
    }

    return false;
}