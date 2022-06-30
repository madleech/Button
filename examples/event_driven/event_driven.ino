#include <Button.h>

Button button1(2); // Connect your button between pin 2 and GND

void onChange()
{
    if (button1.read() == Button::PRESSED)
        Serial.println("Button 3 has been pressed");
    else
        Serial.println("Button 3 has been released");
}

void setup() {
    Serial.begin(9600);
}

void loop () {
    button1.read();
}