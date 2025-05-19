#include <Arduino.h>

#define LED_PIN PD2

void setup() {
  DDRD |= (1 << LED_PIN);
}

void loop() {
  PORTD |= (1 << LED_PIN);
  
  delay(1000);
  
  PORTD &= ~(1 << LED_PIN);
  
  delay(1000);
}
