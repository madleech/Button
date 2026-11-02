#include <Button.h>

Button button(2); // Connect your button between pin 2 and GND

const uint8_t LED = LED_BUILTIN;

bool led_on = false;
bool hold_handled = false; // latch so a single hold toggles the LED only once
uint32_t last_report = 0;

void setup()
{
	button.begin();
	pinMode(LED, OUTPUT);

	while (!Serial)
	{
	}; // for Leos
	Serial.begin(9600);
}

void loop()
{
	// Toggle the LED each time the button is held for a full second.
	// pressed_for() is level-triggered: it stays true on every loop while the
	// button remains held past the threshold. We latch it here so we act on the
	// rising edge only, and re-arm once the button is released.
	if (button.pressed_for(1000))
	{
		if (!hold_handled)
		{
			led_on = !led_on;
			digitalWrite(LED, led_on ? HIGH : LOW);
			Serial.println(led_on ? "LED on" : "LED off");
			hold_handled = true;
		}
	}
	else
	{
		hold_handled = false;
	}

	// While the button is held, report how long every 500ms.
	uint32_t held = button.held_time();
	if (held > 0 && (uint32_t)(millis() - last_report) >= 500)
	{
		last_report = millis();
		Serial.print("Held for ");
		Serial.print(held);
		Serial.println(" ms");
	}
}
