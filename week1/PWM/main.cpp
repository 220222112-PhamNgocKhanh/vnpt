#include <Arduino.h>
const int ledPin = 4;
bool ledState = LOW; // Initial state of the LED
const int buttonPin = 5;
const int PWMFrequency = 5000; // 5 kHz
const int PWMResolution = 8;   // 8-bit resolution
const int PWMChannel = 0;      // Use channel 0 for PWM

void setup()
{
  pinMode(buttonPin, INPUT_PULLDOWN);                 // Set button pin as input with pull-up resistor
  ledcSetup(PWMChannel, PWMFrequency, PWMResolution); // Setup PWM channel
  ledcAttachPin(ledPin, PWMChannel);                  // Attach the PWM channel to the
}

void loop()
{
  while ((1))
  {
    if (digitalRead(buttonPin) == 1)
    {
      if (ledState == 0)
      {
        ledState = 1;
        for (int i = 0; i < 255; i++)
        {
          ledcWrite(PWMChannel, i); // Gradually increase brightness
          delay(10);                // Delay to see the effect
        }
      }
      else if (ledState == 1)
      {
        ledState = 0;
        for (int i = 255; i >= 0; i--)
        {
          ledcWrite(PWMChannel, i); // Gradually decrease brightness
          delay(10);                // Delay to see the effect
        }
      }
    }
  }
}
