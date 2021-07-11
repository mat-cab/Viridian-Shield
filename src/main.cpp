#include <Arduino.h>

#include "debug/debug.h"
#include "inputs/inputs.h"
#include "viridian/viridian.h"
#include "timer/timer.h"
#include "teleinfo/teleinfo.h"

// constants for the main program
// allowed duration in ms to change the charging current (to avoid sending new commands every cycle)
// set to 15 mins (= 900s = 900 000 ms)
const uint32_t MAIN_CHARGE_CYCLE = 900000;
// initial margin for the charing current in Amps
const uint8_t MAIN_INITIAL_MARGIN = 1;
// minimum percentage change to apply the new charging current
const double MAIN_PERCENTAGE_CHANGE_MINIMUM = 0.1;
// initial wait time in ms after the setup
// set to 10s
const uint32_t MAIN_END_SETUP_WAIT = 10000;
// wait time between command change and Teleinfo change
// set to 5s
const uint32_t MAIN_CURRENT_CHANGE_DURATION = 5000;
// main cycle duration in ms
// set to 1s
const uint32_t MAIN_LOOP_DURATION = 1000;
// current to send to the Viridian when no car is charging
const double MAIN_CURRENT_NO_CAR_CHARGING = 10.0;
// Delay after the start of the charge
// set to 15s
const uint32_t MAIN_INITIAL_CHARGE_DELAY = 15000;


void setup() {
  // initialize debug
  debug::initialize();

  // start things up
  debug::log(F("main: Arduino starting"));

  // initialize inputs
  inputs::initialize();

  // initialize the viridian interface
  viridian::initialize();

  // initialize the timer
  timer::setTimerDuration(MAIN_CHARGE_CYCLE);

  // initialize the teleinfo interface
  teleinfo::initialize();

  // debug message to know that initialization is finished
  debug::log(F("main: Initialization finished. Waiting before starting..."));

  // just wait a few seconds before going into the loop, in case we just had an overcurrent protection
  delay(MAIN_END_SETUP_WAIT);

  // debug message to know we finished setup
  debug::log(F("main: Setup finished"));
}

void loop() {
  teleinfo_t teleinfo;

  static boolean chargeStarted;

  // reset the current changed info for the viridian
  viridian::resetChange();
  
  if (inputs::readOption(INPUTS_OPTION_CHARGING_CAR)) {
    // There is no car charging
    debug::log(F("main: No car is charging"));

    // send the appropriate charging current
    viridian::setChargingCurrent(MAIN_CURRENT_NO_CAR_CHARGING);

    // State that the charge did not start
    chargeStarted = false;
  } else {
    if (chargeStarted == false) {
      // log
      debug::log(F("main: Car just started charging, waiting for charge to start"));

      // let the car start charging
      delay(MAIN_INITIAL_CHARGE_DELAY);
    }

    // Read the teleInfo
    debug::log(F("main: Reading teleInfo"));
    teleinfo = teleinfo::read();

    // log the teleinfo data
    debug::logNoLine(F("main: Teleinfo ISOUSC: "));
    debug::log(String(teleinfo.ISOUSC));
    debug::logNoLine(F("main: Teleinfo IINST: "));
    debug::log(String(teleinfo.IINST));

    // If timer is finished or ADPS is received
    if (timer::timerAllows() || teleinfo.ADPS > 0 || chargeStarted == false) {
      if ( chargeStarted == false ) {
        // log
        debug::log(F("main: Now adapting charge current for first charge"));

        // reset the timer
        timer::resetTimer();

        // Record that the charge has started
        chargeStarted = true;
      } else if (teleinfo.ADPS > 0) {
        // log to debug
        debug::log(F("main: ADPS received, adapting charge current"));
      } else {
        // log to debug
        debug::log(F("main: Nominal timer activation"));
      }
      // get the current margin
      // default to 1A + option for the 2A additional margin
      uint8_t currentMargin = MAIN_INITIAL_MARGIN + inputs::readOption(INPUTS_OPTION_MARGIN_ADD_1A);

      // ISOUSC multiplier
      double iSOUSCMultplier = 1.0;

      // If option is set, add 20% margin on ISOUSC
      if (inputs::readOption(INPUTS_OPTION_GREATER_ISOUSC)) {
        iSOUSCMultplier += 0.2;
      }

      // Compute the avalaible current increase
      double availableCurrent = viridian::getChargingCurrent() + (teleinfo.ISOUSC * iSOUSCMultplier - teleinfo.IINST) - currentMargin;

      // additional debug message to understand what is going on
      debug::logNoLine(F("main: available current is now "));
      debug::logNoLine(String(availableCurrent));
      debug::log(F(" Amps"));

      // if we were not charging before, start charging (if it is more than the minimum in viridian module)
      if (viridian::getChargingCurrent() == 0.0) {
        // also do not start charging if no current is available
        if (availableCurrent > 0.0) {
          // set the appropriate charging current
          viridian::setChargingCurrent(availableCurrent);
        }
      } else {
        // check that the availableCurrent is at least one Amp different
        if ((-1.0 < (availableCurrent - viridian::getChargingCurrent())) && ((availableCurrent - viridian::getChargingCurrent()) < 1.0)) {
          // if not, log a message
          debug::log(F("main: Change of charging current is less than one amp. Not changing."));
          } else {
          // compute the percentage change
          double percentageChange = availableCurrent / viridian::getChargingCurrent();

          // if the percentageChange is greater than allowed change
          if (percentageChange > 1 + MAIN_PERCENTAGE_CHANGE_MINIMUM || percentageChange < 1 - MAIN_PERCENTAGE_CHANGE_MINIMUM) {
            // set the new charging current
            viridian::setChargingCurrent(availableCurrent);

            // delay after the change of charge
            delay(MAIN_CURRENT_CHANGE_DURATION);
          } else {
            // log to debug that we did not ask for an update of the charging current
            debug::logNoLine(F("main: Change of charging current is not important enough (already charging at "));
            debug::logNoLine(String(viridian::getChargingCurrent()));
            debug::log(F(" Amps)"));
          }
        }
      }
    } else {
      // simple log message
      debug::log(F("main: waiting for appropriate time to change charge"));
    }
  }

  // wait a bit for the next cycle
  delay(MAIN_LOOP_DURATION);
}