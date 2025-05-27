#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#include "model_weights.h"

#define S0 6
#define S1 7
#define S2 4
#define S3 5
#define sensorOut 8
#define INPUT_PIN PD2     // D2 as input
#define OUTPUT_PIN PD3    // D3 as output
#define ONBOARD_LED PB5   // Onboard LED on D13/PB5

void readColor();

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo ServoCollect;
Servo ServoEject;

enum COLLECT_POS {
    COLLECT_POS_IDLE = 90,
    COLLECT_POS_DETECT = 45,
    COLLECT_POS_EJECT = 0
};

enum EJECT_POS {
    EJECT_POS_IDLE = 0,
    EJECT_POS_GREEN = 0,
    EJECT_POS_ORANGE = 25,
    EJECT_POS_PURPLE = 50,
    EJECT_POS_RED = 75,
    EJECT_POS_YELLOW = 90
};

enum SYSTEM_STATE {
  STATE_IDLE,
  STATE_INITIATE_SORTING,
  STATE_COLLECTING_SAMPLES,
  STATE_CLASSIFY_COLOR,
  STATE_EJECT_ITEM,
  STATE_RESET_POSITION
};

int ejectPosByColor[6] = {
    EJECT_POS_GREEN,  // COLOR_GREEN
    EJECT_POS_ORANGE, // COLOR_ORANGE
    EJECT_POS_PURPLE, // COLOR_PURPLE
    EJECT_POS_RED,    // COLOR_RED
    EJECT_POS_YELLOW, // COLOR_YELLOW
};

int servoPosByPrediction[NUM_CLASSES] = {
  0,   // GREEN
  25,  // ORANGE
  50,  // PURPLE
  75,  // RED
  90   // YELLOW
};

int rf = 0; // Red frequency
int gf = 0; // Green frequency
int bf = 0; // Blue frequency

enum SYSTEM_STATE currentState = STATE_IDLE;

volatile uint8_t sortingRequested = 0;

ISR(INT0_vect) {
    sortingRequested = 1;
}

void setup() {
  Wire.begin();
  delay(100);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("System init...");
  
  // Configure D2 as input with pull-up
  DDRD &= ~(1 << INPUT_PIN);
  PORTD |= (1 << INPUT_PIN);
  
  // Configure D3 as output
  DDRD |= (1 << OUTPUT_PIN);
  
  // Configure onboard LED pin as output
  DDRB |= (1 << ONBOARD_LED);

  EICRA = (EICRA & ~(1 << ISC01)) | (1 << ISC00);  // Any change
  EIFR |= (1 << INTF0);   // Clear pending interrupt
  EIMSK |= (1 << INT0);   // Enable INT0
  sei();                  // Enable global interrupts
  
  // Attach Servo for collecting tube
  ServoCollect.attach(10);
  ServoCollect.write(COLLECT_POS_IDLE);
  
  // Attach Servo for eject tube
  ServoEject.attach(9);
  ServoEject.write(EJECT_POS_IDLE);
  
  // TCS230
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  pinMode(sensorOut, INPUT);
  
  // Set frequency scaling to 100%
  // Options: 0,0 (off), 0,1 (2%), 1,0 (20%), 1,1 (100%)
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  
  // Start serial communication
  Serial.begin(9600);

  sortingRequested = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setup finished");
  lcd.setCursor(0, 1);
  lcd.print("inititialization");
  delay(1000);
}

int samples[NUM_SAMPLES][NUM_FEATURES];
int color_frequency[NUM_CLASSES] = {0};
int max_class = 0;
int max_frequency = -1;

void loop() {

  switch (currentState) {
    case STATE_IDLE:
      if (sortingRequested) {
        sortingRequested = 0;
        lcd.clear();
        lcd.print("System started");
        Serial.println("[LOG] System started...");
        currentState = STATE_INITIATE_SORTING;
      } else {
        lcd.clear();
        lcd.print("System idle..");
        Serial.println("[LOG] System idle, waiting for interrupt to begin...");
        delay(500);
      }
      break;

    case STATE_INITIATE_SORTING:
      ServoCollect.write(COLLECT_POS_DETECT);
      Serial.println("Moving to collect position...");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Collecting");
      lcd.setCursor(0, 1);
      lcd.print(NUM_SAMPLES);
      lcd.print(" samples...");
      Serial.print("[LOG] Started collecting ");
      Serial.print(NUM_SAMPLES);
      Serial.println(" samples...");
      currentState = STATE_COLLECTING_SAMPLES;
      break;

    case STATE_COLLECTING_SAMPLES:
      for (int i = 0; i < NUM_SAMPLES; i++) {
        readColor();
        samples[i][0] = rf;
        samples[i][1] = gf;
        samples[i][2] = bf;
      }
      Serial.println("[LOG] Finished collecting samples.");
      currentState = STATE_CLASSIFY_COLOR;
      break;

    case STATE_CLASSIFY_COLOR:
      max_class = 0;
      max_frequency = -1;

      for (int i = 0; i < NUM_SAMPLES; i++) {
        int predicted_class = predict(samples[i]);
        color_frequency[predicted_class]++;
        if (color_frequency[predicted_class] > max_frequency) {
          max_frequency = color_frequency[predicted_class];
          max_class = predicted_class;
        }
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Color detected:");
      lcd.setCursor(0, 1);
      lcd.print(class_names[max_class]);
      Serial.print("[LOG] Color detected: ");
      Serial.println(class_names[max_class]);
      currentState = STATE_EJECT_ITEM;
      break;

    case STATE_EJECT_ITEM:
      Serial.print("[LOG] Moving eject servo to ");
      Serial.println(class_names[max_class]);
      ServoEject.write(servoPosByPrediction[max_class]);
      delay(1000);
      Serial.println("[LOG] Releasing object...");
      ServoCollect.write(COLLECT_POS_EJECT);
      delay(1000);
      currentState = STATE_RESET_POSITION;
      break;

    case STATE_RESET_POSITION:
      ServoCollect.write(COLLECT_POS_IDLE);
      ServoEject.write(EJECT_POS_IDLE);
      Serial.println("[LOG] Resetting to idle...");
      currentState = STATE_IDLE;
      break;
  }
}

void readColor() {
  // Setting RED filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  rf = pulseIn(sensorOut, LOW);
  Serial.print("");
  Serial.print(rf);
  delay(50);

  // Setting GREEN filtered photodiodes to be read
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  gf = pulseIn(sensorOut, LOW);
  Serial.print(",");
  Serial.print(gf);
  delay(50);

  // Setting BLUE filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  bf = pulseIn(sensorOut, LOW);
  Serial.print(",");
  Serial.print(bf);
  Serial.println("");

  delay(50);
}
