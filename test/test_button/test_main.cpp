/*
    Native unit tests for Button.

    These run off-device via PlatformIO's `native` platform. millis() and
    digitalRead() are mocked (see test/mock/Arduino.h) so we can drive time and
    the pin level deterministically and exercise the debounce logic.

    Wiring model: the button is between the pin and GND with INPUT_PULLUP, so
    the raw pin reads HIGH (RELEASED) when up and LOW (PRESSED) when down.
*/

#include <unity.h>

#include "Arduino.h"
#include "Button.h"

// The mock state declared in test/mock/Arduino.h.
unsigned long _mock_millis = 0;
int _mock_pin_state = HIGH;

unsigned long millis()
{
	return _mock_millis;
}

void pinMode(uint8_t, uint8_t)
{
}

int digitalRead(uint8_t)
{
	return _mock_pin_state;
}

void setUp(void)
{
	_mock_millis = 0;
	_mock_pin_state = HIGH; // released
}

void tearDown(void)
{
}

// A freshly constructed button reads as released.
void test_starts_released(void)
{
	Button button(2);
	TEST_ASSERT_EQUAL(Button::RELEASED, button.read());
}

// Changes within the first _delay ms after construction are debounced away
// (power-on transients are ignored); the button becomes responsive afterwards.
void test_ignores_power_on_window(void)
{
	Button button(2, 100);

	_mock_millis = 50;     // inside the initial debounce window
	_mock_pin_state = LOW; // already held at boot
	TEST_ASSERT_FALSE(button.pressed());

	_mock_millis = 100; // window elapsed
	TEST_ASSERT_TRUE(button.pressed());
}

// pressed() fires exactly once per press.
void test_pressed_fires_once(void)
{
	Button button(2);

	_mock_millis = 1000;   // past the power-on debounce window
	_mock_pin_state = LOW; // button pushed down
	TEST_ASSERT_TRUE(button.pressed());
	TEST_ASSERT_FALSE(button.pressed()); // no new edge
}

// released() fires exactly once when the button comes back up.
void test_released_fires_once(void)
{
	Button button(2);

	_mock_millis = 1000;
	_mock_pin_state = LOW;
	button.pressed(); // register the press (ignore window: 1000..1100)

	_mock_millis = 1150; // past the debounce window
	_mock_pin_state = HIGH;
	TEST_ASSERT_TRUE(button.released());
	TEST_ASSERT_FALSE(button.released());
}

// Bounces inside the debounce window are ignored.
void test_debounce_ignores_bounce(void)
{
	Button button(2);

	_mock_millis = 1000; // press at t=1000, ignore changes until t=1100
	_mock_pin_state = LOW;
	TEST_ASSERT_TRUE(button.pressed());

	_mock_millis = 1050;    // still inside the window
	_mock_pin_state = HIGH; // a contact bounce back up
	TEST_ASSERT_FALSE(button.released());
	TEST_ASSERT_EQUAL(Button::PRESSED, button.read()); // state unchanged
}

// toggled() reports any debounced change of state, once per change.
void test_toggled_on_each_change(void)
{
	Button button(2);

	_mock_millis = 1000;   // past the power-on debounce window
	_mock_pin_state = LOW; // pressed
	TEST_ASSERT_TRUE(button.toggled());
	TEST_ASSERT_FALSE(button.toggled());

	_mock_millis = 1200;    // past debounce window
	_mock_pin_state = HIGH; // released
	TEST_ASSERT_TRUE(button.toggled());
}

// A custom debounce time is honoured.
void test_custom_debounce_time(void)
{
	Button button(2, 500);

	_mock_millis = 1000;
	_mock_pin_state = LOW;
	TEST_ASSERT_TRUE(button.pressed()); // ignore changes until t=1500

	_mock_millis = 1400; // a real release, but still inside the 500ms window
	_mock_pin_state = HIGH;
	TEST_ASSERT_FALSE(button.released());

	_mock_millis = 1500; // window elapsed
	TEST_ASSERT_TRUE(button.released());
}

// Debouncing stays correct across the 32-bit millis() rollover (~49 days).
void test_debounce_survives_millis_wraparound(void)
{
	Button button(2, 100);

	// Prime with a normal press just below the top of the range.
	_mock_millis = 0xFFFFFF00;
	_mock_pin_state = LOW;
	TEST_ASSERT_TRUE(button.pressed());

	// Release just before the rollover. The elapsed time since the press is
	// computed with modular subtraction, so it is correct across the wrap.
	_mock_millis = 0xFFFFFFF0;
	_mock_pin_state = HIGH;
	TEST_ASSERT_TRUE(button.released());

	// A bounce just after millis() wraps to zero is still inside the debounce
	// window (only ~16 ms elapsed since the release) and must be ignored.
	_mock_millis = 0x00000000;
	_mock_pin_state = LOW;
	TEST_ASSERT_FALSE(button.pressed());

	// Once the window has elapsed across the wrap, a genuine press registers.
	_mock_millis = 0x00000080;
	TEST_ASSERT_TRUE(button.pressed());
}

// Reproduces the reported soft-lock: with the old code a change registered near
// the top of the range left a debounce deadline of ~millis_max; after millis()
// wrapped to a small value, (deadline > millis()) stayed true for ~49 days and
// locked out every read. Measuring elapsed time since the change instead means
// the window expires normally across the wrap.
void test_no_softlock_after_overflow(void)
{
	Button button(2, 100);

	// Prime with a normal press, as in normal operation.
	_mock_millis = 0x7FFFFFFF;
	_mock_pin_state = LOW;
	TEST_ASSERT_TRUE(button.pressed());

	// Release at (max - delay): the old deadline (millis + delay) would be max,
	// the exact reported trigger.
	_mock_millis = 0xFFFFFFFF - 100;
	_mock_pin_state = HIGH;
	TEST_ASSERT_TRUE(button.released());

	// millis() rolls over to a small value. A genuine press must still register;
	// the old code would ignore it forever (until millis climbed back to max).
	_mock_millis = 5;
	_mock_pin_state = LOW;
	TEST_ASSERT_TRUE(button.pressed());
}

int main(int, char **)
{
	UNITY_BEGIN();
	RUN_TEST(test_starts_released);
	RUN_TEST(test_ignores_power_on_window);
	RUN_TEST(test_pressed_fires_once);
	RUN_TEST(test_released_fires_once);
	RUN_TEST(test_debounce_ignores_bounce);
	RUN_TEST(test_toggled_on_each_change);
	RUN_TEST(test_custom_debounce_time);
	RUN_TEST(test_debounce_survives_millis_wraparound);
	RUN_TEST(test_no_softlock_after_overflow);
	return UNITY_END();
}
