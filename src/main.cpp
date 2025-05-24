#include <Arduino.h>

#define INPUT_PIN PD2     // D2 as input
#define OUTPUT_PIN PD3    // D3 as output
#define ONBOARD_LED PB5   // Onboard LED on D13/PB5

void setup() {
  // Configure D2 as input with pull-up
  DDRD &= ~(1 << INPUT_PIN);
  PORTD |= (1 << INPUT_PIN);
  
  // Configure D3 as output
  DDRD |= (1 << OUTPUT_PIN);
  
  // Configure onboard LED pin as output
  DDRB |= (1 << ONBOARD_LED);
}

void loop() {
  // Check if input on D2 is 1 (LOW due to pull-up logic being inverted)
  if (!(PIND & (1 << INPUT_PIN))) {
    // Set D3 high
    PORTD |= (1 << OUTPUT_PIN);
    // Turn on onboard LED
    PORTB |= (1 << ONBOARD_LED);
  } else {
    // Set D3 low
    PORTD &= ~(1 << OUTPUT_PIN);
    // Turn off onboard LED
    PORTB &= ~(1 << ONBOARD_LED);
  }
}
