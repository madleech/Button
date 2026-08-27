Button
======

* Author: Michael Adams (<http://www.michael.net.nz>)
* Copyright (C) 2016 Michael D K Adams.
* Released under the MIT license.

Button is a tiny library to make reading buttons very simple. It handles debouncing automatically, and monitoring of state.

Motivation
----------
Ahh buttons. Ahh debouncing! Sometimes you really wish you could ignore all the mechanics of debouncing, reading inputs, monitoring state, and just say... `if (button.pressed())` and know that it'll only run once each time you press a button.

Or how about `if (button.released())`, or `button.toggled()`. It is such a simple concept, but in practice you need to set up timers, monitor input puts, set pullups, etc etc. On anything more than the most simple example, that can become quite a headache.

So fed up with all that I figured there had to be a better way. This library is the result.

Features
--------
* Super simple API.
* Handles debouncing.
* Sets the pin mode automatically.
* Lets you write code that triggers:
** based on the pin state (high or low)
** when a button is pressed
** when a button is released
** or when a button changes (i.e. pressing or releasing)
** when a button has been held down for a length of time

Requirements
------------
* An Arduino — http://arduino.cc/
* A button

Installation
------------
Download the ZIP archive (https://github.com/madleech/Button/zipball/master), then open the Arduino IDE and choose Sketch > Include Library > Add .ZIP Library... and select your downloaded file.

You should now see in File > Examples > Button entires for the basic\_usage and hold\_button examples.

Code Examples
-------------
Here is the 'basic\_usage' example program, included in the download:

```C++
#include <Button.h>

Button button1(2); // Connect your button between pin 2 and GND
Button button2(3); // Connect your button between pin 3 and GND
Button button3(4); // Connect your button between pin 4 and GND

void setup() {
    button1.begin();
    button2.begin();
    button3.begin();

    while (!Serial) { }; // for Leos
    Serial.begin(9600);
}

void loop() {
    if (button1.pressed())
        Serial.println("Button 1 pressed");

    if (button2.released())
        Serial.println("Button 2 released");

    if (button3.toggled()) {
        if (button3.read() == Button::PRESSED)
            Serial.println("Button 3 has been pressed");
        else
            Serial.println("Button 3 has been released");
    }
}
```

And here is a snippet from the 'hold\_button' example, which toggles the built-in LED whenever the button is held down for a second. The full example also reports how long the button has been held for using `held_time()`:

```C++
void loop() {
    // Toggle the LED each time the button is held for a full second.
    // pressed_for() is level-triggered, so we latch it to act once per hold.
    if (button.pressed_for(1000)) {
        if (!hold_handled) {
            led_on = !led_on;
            digitalWrite(LED, led_on ? HIGH : LOW);
            Serial.println(led_on ? "LED on" : "LED off");
            hold_handled = true;
        }
    } else {
        hold_handled = false; // re-arm once released
    }
}
```

Documentation
-------------
**Button(int pin, int debounce_ms = 100)**
Creates a new Button attached a specified pin. If you need to, you can specify the debounce time as a second parameter. If left out, it defaults to 100ms.

**void begin()**
Call this in your `setup` method to setup the button. All it does is set the correct pin mode.

**bool pressed()**
Returns true when _and only when_ the button is pressed. Until the button is released (in the debounced-sense of the word) this function won't return true again. So in effect, it returns true only while you are pressing the button, or to put it another way, it fires on a rising edge.

**bool released()**
Like `pressed()`, but round the other way. So if you hold down a button, and then release it... that is when it fires.

**bool pressed_for(uint32_t duration_ms)**
Returns true while the button has been held down continuously for at least `duration_ms` milliseconds. Unlike `pressed()`, this is _level-triggered_: it keeps returning true on every call for as long as you keep the button held past the threshold. If you want to act only once per hold (e.g. toggle something), latch the result — see the `hold_button` example. Great for "press and hold for one second to..." style interactions.

**uint32_t held_time()**
Returns how many milliseconds the button has been continuously pressed for, or `0` if it isn't currently pressed. Handy if you want to react to the length of a hold rather than a fixed threshold.

**bool toggled()**
Returns true whenever the button is pressed or released, i.e., its position is toggled. To find out what the position actually is, you can use the `read()` function.

**bool read()**
Returns the current debounced state of the button, i.e. Button::PRESSED or Button::RELEASED.

**bool has_changed()**
Returns whether the position/state of the button has changed after calling the previous read() function. Unlikely to be used except by Super Gurus.

Quirks and Things to Keep in Mind
---------------------------------
**Highs and lows, lows and highs**
The easiest way to connect a switch on an Arduino is to connect it between an input pin and ground, and use the internal pullup resistor to make sure it doesn't float. This is fine and dandy, but it can get a bit confusing, as a "pressed" button is logic level: low, while a "released" button is logic level: high.

So to make it a bit more obvious what you're talking about, you can use a couple of handy shortcuts: `Button::PRESSED` and `Button::RELEASED` which map to the expected values.

License
-------
Copyright (c) 2016 Michael D K Adams. http://www.michael.net.nz/

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

