/*
    Minimal Arduino.h mock for native unit tests.

    Provides just enough of the Arduino runtime for Button to build and run
    off-device. The test controls time via _mock_millis and the (single) pin
    level via _mock_pin_state, both defined in the test translation unit.
*/

#ifndef _Button_test_Arduino_h
#define _Button_test_Arduino_h

#include <stdint.h>
#include <stddef.h>

#define LOW 0
#define HIGH 1
#define INPUT_PULLUP 2

// Controllable test state, defined by the test translation unit.
extern unsigned long _mock_millis;
extern int _mock_pin_state;

unsigned long millis();
void pinMode(uint8_t pin, uint8_t mode);
int digitalRead(uint8_t pin);

#endif
