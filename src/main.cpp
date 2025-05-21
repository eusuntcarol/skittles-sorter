#include <Arduino.h>

// Define pins
#define S0 6
#define S1 7
#define S2 4
#define S3 5
#define sensorOut 8

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

int redColor = 0;
int greenColor = 0;
int blueColor = 0;

void readSensor();
void detectColor();

void setup() {
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