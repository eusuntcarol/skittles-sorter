#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin();
  delay(100);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("TEST DISPLAY");
}

void loop() {
  lcd.backlight();
  delay(1000);
  lcd.noBacklight();
  delay(1000);
}
