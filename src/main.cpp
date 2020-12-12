#include <Arduino.h>

#include <teleInfo.h>

#include "debug/debug.h"
#include "inputs/inputs.h"
#include "viridian/viridian.h"
#include "timer/timer.h"

// constants for the main program
// allowed duration in ms to change the charging current (to avoid sending new commands every cycle)
// set to 30 mins (= 1800s = 1 800 000 ms)
const uint32_t MAIN_CHARGE_CYCLE = 1800000;
// initial margin for the charing current in Amps
const uint8_t MAIN_INITIAL_MARGIN = 1;
// additional margin in Amps for the option
const uint8_t MAIN_ADDITIONAL_OPTION_MARGIN = 2;
// minimum percentage change to apply the new charging current
const double MAIN_PERCENTAGE_CHANGE_MINIMUM = 0.1;
// initial wait time in ms after the setup
// set to 10s
const uint32_t MAIN_END_SETUP_WAIT = 10000;
// main cycle duration in ms
// set to 1s
const uint32_t MAIN_LOOP_DURATION = 1000;

// Create the teleInfo Object on pin 0
teleInfo TI(0);

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

  // debug message to know we finished setup
  debug::log("main: Setup finished");

  // just wait a few seconds before going into the loop, in case we just had an overcurrent protection
  delay(MAIN_END_SETUP_WAIT);
}

void loop() {
  // Read the teleInfo
  debug::log("main: Reading teleInfo");
  teleInfo_t teleInfo = TI.get();

  // log the teleinfo data
  debug::log("main: Teleinfo ISOUSC: "+String(teleInfo.ISOUSC));
  debug::log("main: Teleinfo IINST: "+String(teleInfo.IINST));

  // If timer is finished or ADPS is received
  if (timer::timerAllows() || teleInfo.ADPS > 0 ) {
    // If there is an ADPS
    if (teleInfo.ADPS > 0 ) {
      // log to debug
      debug::log("main: ADPS received, stopped charging");
    } else {
      // log to debug
      debug::log("main: Nominal timer activation");
    }
    // get the current margin
    // default to 1A + option for the 2A additional margin
    uint8_t currentMargin = MAIN_INITIAL_MARGIN + inputs::readOption(INPUTS_OPTION_MARGIN_ADD_2A) * MAIN_ADDITIONAL_OPTION_MARGIN;

    // Compute the avalaible current increase
    double availableCurrent = viridian::getChargingCurrent() + (teleInfo.ISOUSC - teleInfo.IINST) - currentMargin;

    // additional debug message to understand what is going on
    debug::log("main: available current is now "+String(availableCurrent)+ " Amps");

    // if we were not charging before, start charging (if it is more than the minimum in viridian module)
    if (viridian::getChargingCurrent() == 0.0) {
      // set the appropriate charging current
      viridian::setChargingCurrent(availableCurrent);
    } else {
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
  } else {
    // simple log message
    debug::log("main: waiting for appropriate time to change charge");
  }

  // wait a bit for the next cycle
  delay(MAIN_LOOP_DURATION);
}