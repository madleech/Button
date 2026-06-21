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

// pressed() fires exactly once per press.
void test_pressed_fires_once(void)
{
	Button button(2);

	_mock_pin_state = LOW; // button pushed down
	TEST_ASSERT_TRUE(button.pressed());
	TEST_ASSERT_FALSE(button.pressed()); // no new edge
}

// released() fires exactly once when the button comes back up.
void test_released_fires_once(void)
{
	Button button(2);

	_mock_pin_state = LOW;
	button.pressed(); // register the press (debounce window: 0..100)

	_mock_millis = 150; // past the debounce window
	_mock_pin_state = HIGH;
	TEST_ASSERT_TRUE(button.released());
	TEST_ASSERT_FALSE(button.released());
}

// Bounces inside the debounce window are ignored.
void test_debounce_ignores_bounce(void)
{
	Button button(2);

	_mock_pin_state = LOW; // press at t=0, ignore changes until t=100
	TEST_ASSERT_TRUE(button.pressed());

	_mock_millis = 50;      // still inside the window
	_mock_pin_state = HIGH; // a contact bounce back up
	TEST_ASSERT_FALSE(button.released());
	TEST_ASSERT_EQUAL(Button::PRESSED, button.read()); // state unchanged
}

// toggled() reports any debounced change of state, once per change.
void test_toggled_on_each_change(void)
{
	Button button(2);

	_mock_pin_state = LOW; // pressed
	TEST_ASSERT_TRUE(button.toggled());
	TEST_ASSERT_FALSE(button.toggled());

	_mock_millis = 200;     // past debounce window
	_mock_pin_state = HIGH; // released
	TEST_ASSERT_TRUE(button.toggled());
}

// A custom debounce time is honoured.
void test_custom_debounce_time(void)
{
	Button button(2, 500);

	_mock_pin_state = LOW;
	TEST_ASSERT_TRUE(button.pressed()); // ignore changes until t=500

	_mock_millis = 400; // a real release, but still inside the 500ms window
	_mock_pin_state = HIGH;
	TEST_ASSERT_FALSE(button.released());

	_mock_millis = 500; // window elapsed
	TEST_ASSERT_TRUE(button.released());
}

int main(int, char **)
{
	UNITY_BEGIN();
	RUN_TEST(test_starts_released);
	RUN_TEST(test_pressed_fires_once);
	RUN_TEST(test_released_fires_once);
	RUN_TEST(test_debounce_ignores_bounce);
	RUN_TEST(test_toggled_on_each_change);
	RUN_TEST(test_custom_debounce_time);
	return UNITY_END();
}
