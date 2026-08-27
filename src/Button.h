/*
    Button - a small library for Arduino to handle button debouncing

    MIT licensed.
*/

#ifndef Button_h
#define Button_h
#include "Arduino.h"

class Button
{
  public:
	Button(uint8_t pin, uint16_t debounce_ms = 100);
	void begin();
	bool read();
	bool toggled();
	bool pressed();
	bool released();
	bool pressed_for(uint32_t duration_ms);
	uint32_t held_time();
	bool has_changed();

	const static bool PRESSED = LOW;
	const static bool RELEASED = HIGH;

  private:
	// ordered largest-first so the struct packs without alignment padding on
	// 32-bit targets (12 bytes instead of 16; no effect on 8-bit AVR, which is
	// unaligned and always 9 bytes)
	uint32_t _ignore_start;
	uint16_t _delay;
	uint8_t _pin;
	bool _state;
	bool _has_changed;
};

#endif
