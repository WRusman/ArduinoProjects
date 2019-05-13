#include <Servo.h>  

#define thOutPin 2
#define stOutPin 3
#define thInPin 4
#define stInPin 5

Servo thServo, stServo;

void setup() {
  pinMode(thInPin,INPUT);
  pinMode(stInPin,INPUT);

  thServo.attach(thOutPin);
  stServo.attach(stOutPin);
}

void loop() {
  thServo.writeMicroseconds((pulseIn(thInPin, HIGH)) / 2); 
  stServo.writeMicroseconds((pulseIn(stInPin, HIGH)) / 2); 
}
