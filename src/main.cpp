#include <Arduino.h>
#include <BleMouse.h>

// Create a BLE mouse object with a custom device name and manufacturer
BleMouse bleMouse("ESP32 Mouse", "THE ALFA SIGMA GIGACHAD GmbH", 100);

#define JOYSTICK_X 25
#define JOYSTICK_Y 26
#define JOYSTICK_BTN 4

int maxValuel = 10;

int errorX = 0;
int errorY = 0;

void setup()
{
	Serial.begin(115200);

	// Initialize the Joystick
	pinMode(JOYSTICK_X, INPUT);
	pinMode(JOYSTICK_Y, INPUT);
	pinMode(JOYSTICK_BTN, INPUT_PULLUP);

	// Initialize BLE Mouse
	Serial.println("Starting BLE Mouse...");
	bleMouse.begin();
	Serial.println("BLE Mouse Initialized.");

	// error
	int h = analogRead(JOYSTICK_X);
	int v = analogRead(JOYSTICK_Y);

	errorX = map(h, 0, 4095, -maxValuel, maxValuel);
	errorY = map(v, 0, 4095, -maxValuel, maxValuel);
}

void loop()
{
	int h = analogRead(JOYSTICK_X);
	int v = analogRead(JOYSTICK_Y);
	bool click = digitalRead(JOYSTICK_BTN) == LOW; // Button pressed when LOW

	// Map analog values to mouse movement range
	int xOffset = map(h, 0, 4095, -maxValuel, maxValuel) - errorX;
	int yOffset = map(v, 0, 4095, -maxValuel, maxValuel) - errorY;

	// Print joystick values
	Serial.print("H: ");
	Serial.println(xOffset);
	Serial.print("V: ");
	Serial.println(yOffset);
	Serial.print("BTN: ");
	Serial.println(click ? "Pressed" : "Released");

	int treshhold = 5;
	// Move the mouse if there's significant movement
	if (abs(xOffset) > treshhold || abs(yOffset) > treshhold)
	{
		bleMouse.move(xOffset, yOffset);
	}

	// Handle mouse click
	if (click)
	{
		if (!bleMouse.isPressed(MOUSE_LEFT))
		{
			bleMouse.press(MOUSE_LEFT);
		}
	}
	else
	{
		if (bleMouse.isPressed(MOUSE_LEFT))
		{
			bleMouse.release(MOUSE_LEFT);
		}
	}

	delay(20);
}
