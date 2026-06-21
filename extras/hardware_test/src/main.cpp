/*
    Hardware smoke-test for Button.

    Wire a button between pin 2 and GND (the internal pull-up is enabled, so no
    external resistor is needed). The onboard LED mirrors the debounced state -
    lit while the button is held - and each edge is reported over Serial:

      "pressed"  on a debounced press
      "released" on a debounced release

    Upload + monitor:
      pio run -d extras/hardware_test -e uno -t upload
      pio device monitor -b 9600
*/

#include <Arduino.h>
#include <Button.h>

const uint8_t BUTTON_PIN = 2;
Button button(BUTTON_PIN);

void setup()
{
	button.begin();
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	Serial.println(F("Button hardware test - press the button on pin 2"));
}

void loop()
{
	if (button.pressed())
		Serial.println(F("pressed"));

	if (button.released())
		Serial.println(F("released"));

	// Mirror the debounced state on the LED (PRESSED == LOW).
	digitalWrite(LED_BUILTIN, button.read() == Button::PRESSED ? HIGH : LOW);
}
