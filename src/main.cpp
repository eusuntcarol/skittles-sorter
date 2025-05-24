#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define S0 6
#define S1 7
#define S2 4
#define S3 5
#define sensorOut 8
#define INPUT_PIN PD2     // D2 as input
#define OUTPUT_PIN PD3    // D3 as output
#define ONBOARD_LED PB5   // Onboard LED on D13/PB5

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

int redColor = 0;
int greenColor = 0;
int blueColor = 0;

void readSensor();
void detectColor();

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo ServoM1;
Servo ServoM2;

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
  
  // Attach Servo M1
  ServoM1.attach(9);
  
  // Attach Servo M2
  ServoM2.attach(10);
  
  // TCS230
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  pinMode(sensorOut, INPUT);
  
  // Set frequency scaling to 100%
  // Options: 0,0 (off), 0,1 (2%), 1,0 (20%), 1,1 (100%)
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);
  
  // Start serial communication
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  lcd.backlight();
  delay(1000);
  lcd.noBacklight();
  delay(1000);

  ServoM1.write(0);
  ServoM2.write(0);
  delay(1000);

  ServoM1.write(180);
  ServoM2.write(180);
  
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
  
  readSensor();
  
  Serial.print("R = ");
  Serial.print(redColor);
  Serial.print(" | G = ");
  Serial.print(greenColor);
  Serial.print(" | B = ");
  Serial.println(blueColor);
  
  detectColor();
  
  delay(500);
}

void readSensor() {
  // Setting RED filter photodiodes
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  
  redFrequency = pulseIn(sensorOut, LOW);
  redColor = map(redFrequency, 25, 70, 255, 0);
  redColor = constrain(redColor, 0, 255);
  
  delay(50);
  
  // Setting GREEN filter photodiodes
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  
  greenFrequency = pulseIn(sensorOut, LOW);
  greenColor = map(greenFrequency, 30, 90, 255, 0);
  greenColor = constrain(greenColor, 0, 255);
  
  delay(50);
  
  // Setting BLUE filter photodiodes
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  
  blueFrequency = pulseIn(sensorOut, LOW);
  blueColor = map(blueFrequency, 25, 70, 255, 0);
  blueColor = constrain(blueColor, 0, 255);
}

void detectColor() {
  if (redColor > greenColor && redColor > blueColor && redColor > 40) {
    Serial.println("Detected color: RED");
  } else if (greenColor > redColor && greenColor > blueColor && greenColor > 40) {
    Serial.println("Detected color: GREEN");
  } else if (blueColor > redColor && blueColor > greenColor && blueColor > 40) {
    Serial.println("Detected color: BLUE");
  } else if (redColor > 200 && greenColor > 200 && blueColor > 200) {
    Serial.println("Detected color: WHITE");
  } else if (redColor < 40 && greenColor < 40 && blueColor < 40) {
    Serial.println("Detected color: BLACK");
  } else {
    Serial.println("Detected color: UNKNOWN");
  }
}