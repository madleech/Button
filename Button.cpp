/*
	Button - a small library for Arduino to handle button debouncing
	
	MIT licensed.
*/

#include "Button.h"
#include <Arduino.h>

Button::Button(uint8_t pin, uint16_t debounce_ms)
:  _pin(pin)
,  _delay(debounce_ms)
,  _state(HIGH)
,  _debounce_state(_state)
,  _ignore_until(0)
,  _has_changed(false)
{
}

void Button::begin()
{
	pinMode(_pin, INPUT_PULLUP);
}

// 
// public methods
// 

bool Button::read()
{
	// monitor pin changes during debounce time 
	if (digitalRead(_pin) != _state)
	{
		_state = !_state;
		_ignore_until = millis() + _delay;
	}
	// if no changes in debounce time, the button state stable, update value
	else if (_ignore_until > millis())
	{
		_has_changed = (_debounce_state != _state);
		_debounce_state = _state;
	}
	
	return _debounce_state;
}

// has the button been toggled from on -> off, or vice versa
bool Button::toggled()
{
	read();
	return has_changed();
}

// mostly internal, tells you if a button has changed after calling the read() function
bool Button::has_changed()
{
	if (_has_changed)
	{
		_has_changed = false;
		return true;
	}
	return false;
}

// has the button gone from off -> on
bool Button::pressed()
{
	return (read() == PRESSED && has_changed());
}

// has the button gone from on -> off
bool Button::released()
{
	return (read() == RELEASED && has_changed());
}
