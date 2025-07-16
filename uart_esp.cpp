#include <Arduino.h>
void setup() {
  Serial.begin(9600);       // UART0, debug
  Serial1.begin(9600, SERIAL_8N1, 16, 17);  // UART1: TX = GPIO17, RX = GPIO16
}

void loop() {
  char c;
  if (Serial.available()) {
    c = Serial.read();
    Serial1.println(c); // Forward data from UART0 to UART1
  }
}




