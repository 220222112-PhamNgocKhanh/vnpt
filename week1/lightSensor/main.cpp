#include <Arduino.h>
const int ledPin = 5; 
const int sensorPin = 4; 

void setup() {
  pinMode(ledPin, OUTPUT); // Set the LED pin as output
  pinMode(sensorPin, INPUT); // Set the sensor pin as input
}

void loop() {
  if (digitalRead(sensorPin) == HIGH) { // If the sensor is triggered
    digitalWrite(ledPin, HIGH); // Turn on the LED
  } else {
    digitalWrite(ledPin, LOW); // Turn off the LED
  }
}

