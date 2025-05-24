#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define INPUT_PIN PD2     // D2 as input
#define OUTPUT_PIN PD3    // D3 as output
#define ONBOARD_LED PB5   // Onboard LED on D13/PB5

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {
  Wire.begin();
  delay(100);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("TEST DISPLAY");
  
  // Configure D2 as input with pull-up
  DDRD &= ~(1 << INPUT_PIN);
  PORTD |= (1 << INPUT_PIN);
  
  // Configure D3 as output
  DDRD |= (1 << OUTPUT_PIN);
  
  // Configure onboard LED pin as output
  DDRB |= (1 << ONBOARD_LED);
}

void loop() {
  lcd.backlight();
  delay(1000);
  lcd.noBacklight();
  delay(1000);

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
