#include <Arduino.h>

#include <SoftwareSerial.h>

#include "debug/debug.h"

#include "teleinfo.h"

char teleinfo::labelBuffer[TELEINFO_LABEL_BUFFER_SIZE];
char teleinfo::valueBuffer[TELEINFO_VALUE_BUFFER_SIZE];

// Use same pin for RX and TX since we will not send data
SoftwareSerial sSerial(TELEINFO_INPUT_PIN, TELEINFO_INPUT_PIN);

void teleinfo::initialize() {
    // clear the buffer
    teleinfo::clearBuffer();

    // start Serial
    sSerial.begin(1200);
}

teleinfo_t teleinfo::read() {
    // structure for the return
    teleinfo_t result;

    // Begin by flushing the serial interface
    while (sSerial.available())
        sSerial.read();
    debug::log(F("teleinfo: sSerial.flushed"));

    // Find the end of the ongoing teleinfo frame
    do {
        // try to read a line            
        if (!teleinfo::readLine()) {
            // if for some reason it failed, log so
            debug::log(F("teleinfo: impossible to synchronize with start of teleinfo frame"));

            // and return the (empty) teleinfo data
            return result;
        }
    } while (strcmp(teleinfo::labelBuffer, "MOTDETAT") != 0);
    debug::log(F("teleinfo: found end of teleinfo frame"));

    // we are at the start of a frame
    do {
        if (teleinfo::readLine()) {                
            // try to match the label with all available labels
            if (record("ADCO", result.ADCO)) continue;
            if (record("OPTARIF", result.OPTARIF)) continue;
            if (record("ISOUSC", result.ISOUSC)) continue;
            if (record("PTEC", result.PTEC)) continue;
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
            if (record("HCHC", result.HCHC)) continue;
            if (record("HCHP", result.HCHP)) continue;
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
            debug::logNoLine(F("teleinfo: Unknown label found - label: "));
            debug::logNoLine(String(teleinfo::labelBuffer));
            debug::logNoLine(F(" - value: "));
            debug::log(String(teleinfo::valueBuffer));
        } else {
            debug::log(F("teleinfo: read failed"));
        }
    } while (strcmp(teleinfo::labelBuffer, "MOTDETAT") != 0);

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
    uint32_t timeoutStart;

    // clear the buffer
    teleinfo::clearBuffer();

    // initiate the timeout counter
    timeoutStart = millis();

    // First make sure that we are at the end of a line
    do {
        while (!sSerial.available() && (millis() - timeoutStart) < TELEINFO_TIMEOUT) ;

        // check if timeout is reached
        if (millis() - timeoutStart >= TELEINFO_TIMEOUT) {
            // debug that the read failed in timeout
            debug::log(F("teleinfo: Timeout on teleinfo read"));

            // return it failed
            return false;
        }
    } while (teleinfo::readChar() != '\n');

    // First read the label
    if (teleinfo::readWord(teleinfo::labelBuffer, TELEINFO_LABEL_BUFFER_SIZE, cks) ) {
        debug::logNoLine(F("teleinfo: read label "));
        debug::log(String(teleinfo::labelBuffer));
        // Next read the value
        if (teleinfo::readWord(teleinfo::valueBuffer, TELEINFO_VALUE_BUFFER_SIZE, cks)) {
            debug::logNoLine(F("teleinfo: read value "));
            debug::log(String(teleinfo::valueBuffer));
            // Read the final cks
            while (!sSerial.available()) ;
            messageCks = teleinfo::readChar();

            // compute our own cks
            cks = (cks & 0x3F) + 0x20;

            // return whether the cks are identical
            return (cks==messageCks);
        } else {
            // log an issue
            debug::log(F("teleinfo: error while reading a value"));
        }
    } else {
        // log an issue
        debug::log(F("teleinfo: error while reading a label"));
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
        while (!sSerial.available()) ;

        // read one char
        c = teleinfo::readChar();

        // compute cks
        cks += (uint8_t)c;

        // assign to buffer (if not empty)
        if (c != ' ')  {
            buffer[counter++] = c;
        }
    } while (c != ' ' && counter < maxBufferLength);

    if (c != ' ' && counter == maxBufferLength) {
        // check for buffer overflow
        debug::log(F("teleinfo: buffer overflow !!! buffer cleared"));
        debug::logNoLine(F("teleinfo: buffer was "));
        debug::log(String(buffer));

        // clear the buffer
        teleinfo::clearBuffer();
    }

    // return whether the buffer overflowed or not
    return (counter != maxBufferLength);
}

char teleinfo::readChar() {
    // Read a single char
    // the 0x7F mask is necessary because it is not possible to begin the softwareSerial with parameters SERIAL_7E1 
    return sSerial.read() & 0x7F;
}

bool teleinfo::record(const char* label, char* destination) {
    // compare the label with the expected label
    if (strcmp(teleinfo::labelBuffer, label) == 0) {
        // copy the value to the destination
        strcpy(destination, teleinfo::valueBuffer);
        // return OK
        return true;
    }

    return false;
}

bool teleinfo::record(const char* label, uint8_t &destination) {
    // compare the label with the expected label
    if (strcmp(teleinfo::labelBuffer, label) == 0) {
        // convert the value to the destination
        destination = atoi(teleinfo::valueBuffer);
        // return OK
        return true;
    }

    return false;
}

bool teleinfo::record(const char* label, uint32_t &destination) {
    // compare the label with the expected label
    if (strcmp(teleinfo::labelBuffer, label) == 0) {
        // convert the value to the destination
        destination = atol(teleinfo::valueBuffer);
        // return OK
        return true;
    }

    return false;
}