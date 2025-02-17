#include <Arduino.h>
#include <BleMouse.h>

// Create a BLE mouse object with a custom device name and manufacturer
BleMouse bleMouse("ESP32 Mouse", "THE ALFA SIGMA GIGACHAD GmbH", 100);

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE Mouse...");

    // Initialize the BLE mouse (this starts advertising)
    bleMouse.begin();
}

void loop()
{
    // Check if a central device (phone, computer, etc.) is connected
    if (bleMouse.isConnected())
    {
        Serial.println("Connected. Moving mouse...");

        // Move the mouse 10 units to the right and 10 units down
        bleMouse.move(10, 10);
        delay(1000); // Move the mouse every second
    }
    delay(10);
}
