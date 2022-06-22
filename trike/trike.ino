#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define NUM_LEDS 48
#define throttlePin A7    // input of the hall sensor in the throttle handle
#define enableRelay 2     // power_switch to the controller
#define lightRelay 4      // power_switch to the controller
#define pwmPin 5          // pwm output to the controller
#define servoPin 7        // servo output for throttle potmeter of the controller
#define brakeSwitch 8     // brake switch input (to Gnd)
#define rearLight 9       // ws2812 output to the rear lights
#define lBSwitch 10       // left blinker switch input (to Gnd)
#define rBSwitch 11       // right blinker switch input (to Gnd)
#define enableSwitch 12   // enable switch input (to Gnd)

int loopCounter;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, rearLight, NEO_GRB + NEO_KHZ800);
Servo throttleServo;

void setup() {
  pinMode(pwmPin,OUTPUT);
  pinMode(servoPin,OUTPUT);
  pinMode(enableRelay,OUTPUT);
  pinMode(lightRelay,OUTPUT);
  pinMode(rearLight,OUTPUT);
  pinMode(lBSwitch,INPUT);
  pinMode(rBSwitch,INPUT);
  pinMode(enableSwitch,INPUT);
  pinMode(brakeSwitch,INPUT);
  pinMode(throttlePin,INPUT);
  
  throttleServo.attach(servoPin);
  digitalWrite(enableRelay,0);
  digitalWrite(lightRelay,0);
  digitalWrite(rearLight,0);
  digitalWrite(enableRelay,1);
  strip.begin();
  loopCounter=0;
}

// the loop routine runs over and over again forever:
void loop() {
  enableThread();
  blinkerThread();
  brakeThread();
  delay(30);
}

void enableThread(){
    analogWrite(pwmPin,map(analogRead(throttlePin),180,870,55,255));
    throttleServo.write(map(analogRead(throttlePin),180,870,90,180));
}

void blinkerThread(){

  if (digitalRead(lBSwitch)){                                // left blinker activated
    if (loopCounter == 0) {
      for(int i = 0; i < 20; i++ ) {
        strip.setPixelColor(i,0,0,0);
      }
    }  
    else {
      strip.setPixelColor(20-loopCounter,200,50,0);
    }
  }
 else{                                          // left blinker NOT activated
    for(int i = 0; i < 20; i++ ) {
      strip.setPixelColor(i,20,0,0);
    }
  }

  if (digitalRead(rBSwitch)){                               // right blinker activated
    if (loopCounter == 0) {
      for(int i = 29; i <= 48; i++ ) {
        strip.setPixelColor(i,0,0,0);
      }
    }  
    else
      strip.setPixelColor(28+(loopCounter),200,50,0);
    }
  else {                                          // right blinker NOT activated
      for(int i = 29; i <= 48; i++ ) {
        strip.setPixelColor(i,20,0,0);
      }
    }
  strip.show();
  loopCounter++;
  if (loopCounter==21){loopCounter=0;};
}

void brakeThread() {
 if (digitalRead(brakeSwitch)){                                   // brake activated
    for(int i = 20; i <= 28; i++ ) {
      strip.setPixelColor(i,255,0,0);
    }
  }
  else {                                         // brake NOT activated
    for(int i = 20; i <= 28; i++ ) {
    strip.setPixelColor(i,0,0,0);
    }
  }
  strip.show();
}

  
