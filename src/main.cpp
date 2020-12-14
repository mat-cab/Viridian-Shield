#include <Arduino.h>

#include "debug/debug.h"
#include "inputs/inputs.h"
#include "viridian/viridian.h"
#include "timer/timer.h"
#include "teleinfo/teleinfo.h"

// constants for the main program
// allowed duration in ms to change the charging current (to avoid sending new commands every cycle)
// set to 30 mins (= 1800s = 1 800 000 ms)
const uint32_t MAIN_CHARGE_CYCLE = 1800000;
// initial margin for the charing current in Amps
const uint8_t MAIN_INITIAL_MARGIN = 1;
// minimum percentage change to apply the new charging current
const double MAIN_PERCENTAGE_CHANGE_MINIMUM = 0.1;
// initial wait time in ms after the setup
// set to 10s
const uint32_t MAIN_END_SETUP_WAIT = 10000;
// wait time between command change and Teleinfo change
// set to 1s
const uint32_t MAIN_CURRENT_CHANGE_DURATION = 1000;
// main cycle duration in ms
// set to 1s
const uint32_t MAIN_LOOP_DURATION = 1000;

void setup() {
  // initialize debug
  debug::initialize();

  // start things up
  debug::log("main: Arduino starting");

  // initialize inputs
  inputs::initialize();

  // initialize the viridian interface
  viridian::initialize();

  // initialize the timer
  timer::setTimerDuration(MAIN_CHARGE_CYCLE);

  // initialize the teleinfo interface
  teleinfo::initialize();

  // debug message to know we finished setup
  debug::log("main: Setup finished");

  // just wait a few seconds before going into the loop, in case we just had an overcurrent protection
  delay(MAIN_END_SETUP_WAIT);
}

void loop() {
  // Read the teleInfo
  debug::log("main: Reading teleInfo");
  teleinfo_t teleinfo = teleinfo::read();

  // log the teleinfo data
  debug::log("main: Teleinfo ISOUSC: "+String(teleinfo.ISOUSC));
  debug::log("main: Teleinfo IINST: "+String(teleinfo.IINST));

  // reset the current changed info for the viridian
  viridian::resetChange();

  // If timer is finished or ADPS is received
  if (timer::timerAllows() || teleinfo.ADPS > 0 ) {
    // If there is an ADPS
    if (teleinfo.ADPS > 0 ) {
      // log to debug
      debug::log("main: ADPS received, adapting charge current");
    } else {
      // log to debug
      debug::log("main: Nominal timer activation");
    }
    // get the current margin
    // default to 1A + option for the 2A additional margin
    uint8_t currentMargin = MAIN_INITIAL_MARGIN + inputs::readOption(INPUTS_OPTION_MARGIN_ADD_2A) * 2 + inputs::readOption(INPUTS_OPTION_MARGIN_ADD_1A);

    // Compute the avalaible current increase
    double availableCurrent = viridian::getChargingCurrent() + (teleinfo.ISOUSC - teleinfo.IINST) - currentMargin;

    // additional debug message to understand what is going on
    debug::log("main: available current is now "+String(availableCurrent)+ " Amps");

    // if we were not charging before, start charging (if it is more than the minimum in viridian module)
    if (viridian::getChargingCurrent() == 0.0) {
      // also do not start charging if no current is available
      if (availableCurrent > 0.0) {
        // set the appropriate charging current
        viridian::setChargingCurrent(availableCurrent);
      }
    } else {
      // check that the availableCurrent is at least one Amp different
      if ( (availableCurrent - viridian::getChargingCurrent()) < 1.0 || (availableCurrent - viridian::getChargingCurrent()) > -1.0) {
        // if not, log a message
        debug::log("main: Change of charging current is less than one amp. Not changing.");
      }  else {
        // compute the percentage change
        double percentageChange = availableCurrent / viridian::getChargingCurrent();

        // if the percentageChange is greater than allowed change
        if (percentageChange > 1 + MAIN_PERCENTAGE_CHANGE_MINIMUM || percentageChange < 1 - MAIN_PERCENTAGE_CHANGE_MINIMUM) {
          // set the new charging current
          viridian::setChargingCurrent(availableCurrent);
        } else {
          // log to debug that we did not ask for an update of the charging current
          debug::log("main: Change of charging current is not important enough (already charging at "+String(viridian::getChargingCurrent())+" Amps)");
        }
      }
    }
  } else {
    // simple log message
    debug::log("main: waiting for appropriate time to change charge");
  }

  // check if the current changed at that cycle
  if (viridian::currentChanged()) {
    // wait for a bit so that the current is established
    delay(MAIN_CURRENT_CHANGE_DURATION);

    // read the teleinfo
    teleinfo_t newTeleInfo = teleinfo::read();

    // check that the IINST has changed
    if (newTeleInfo.IINST == teleinfo.IINST) {
      // IINST has not changed, most likely we are not connected
      debug::log("main: no change of IINST, charger is probably offline. Stopping charge");
      // reset to no charging current
      viridian::stopCharging();
    } 
  }

  // wait a bit for the next cycle
  delay(MAIN_LOOP_DURATION);
}