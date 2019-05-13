#include <Adafruit_SoftServo.h>  

unsigned long thIn,stIn;
long outAngle;

#define thInPin 0
#define stInPin 1
#define thOutPin 3
#define stOutPin 4

Adafruit_SoftServo thServo, stServo;

void setup() {
    // Set up the interrupt that will refresh the servo for us automagically
  OCR0A = 0xAF;            // any number is OK
  TIMSK |= _BV(OCIE0A);    // Turn on the compare interrupt (below!)
  pinMode(thInPin,INPUT);
  pinMode(stInPin,INPUT);
  pinMode(thOutPin,OUTPUT);
  pinMode(stOutPin,OUTPUT);

  thServo.attach(thOutPin);
  stServo.attach(stOutPin);
  thServo.write(90);
  stServo.write(90);
  delay(15);
}

void loop() {
  thIn = pulseIn(thInPin, HIGH);
  outAngle= map((thIn), 1000, 2000, 0, 180);
  thServo.write(outAngle); 
  stIn = pulseIn(stInPin, HIGH);
  outAngle= map((stIn), 1000, 2000, 0, 180);
  stServo.write(outAngle); 
    delay(15);
}

// We'll take advantage of the built in millis() timer that goes off
// to keep track of time, and refresh the servo every 20 milliseconds
volatile uint8_t counter = 0;
SIGNAL(TIMER0_COMPA_vect) {
  // this gets called every 2 milliseconds
  counter += 2;
  // every 20 milliseconds, refresh the servos!
  if (counter >= 20) {
    counter = 0;
    thServo.refresh();
    stServo.refresh();
  }
}
