#include <Arduino.h>
#include <BleKeyboard.h>

// Create a BLE keyboard object with a custom device name and manufacturer
BleKeyboard bleKeyboard("ESP32 Keyboard", "THE ALFA SIGMA GIGACHAD GmbH", 100);

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE Keyboard...");

    // Initialize the BLE keyboard
    bleKeyboard.begin();
}

void loop()
{
    // Check if a central device (phone, computer, etc.) is connected
    if (bleKeyboard.isConnected())
    {
        Serial.println("Connected. Sending keystrokes...");
        bleKeyboard.print("Hello, World!");
        delay(1000); // Send the message every second
    }
    delay(10);
}