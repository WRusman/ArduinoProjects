#include <Servo.h>
Servo throttleServo;
int throttlePin=A0;
int servoPin=3;
int enableRelay=4;
int lBlinker=5;
int rBlinker=6;
int enableSwitch=13;
int lBSwitch=9;
int rBSwitch=10;
unsigned long startTimer = 0;
unsigned long currentTimer = 0;

bool enabled,lBEnabled,rBEnabled,timerSet;

void setup() {
    pinMode(servoPin,OUTPUT);
    pinMode(enableRelay,OUTPUT);
    pinMode(lBlinker,OUTPUT);
    pinMode(rBlinker,OUTPUT);
    pinMode(lBSwitch,INPUT);
    pinMode(rBSwitch,INPUT);
    pinMode(enableSwitch,INPUT);
    pinMode(throttlePin,INPUT);
    digitalWrite(lBlinker,0);
    digitalWrite(rBlinker,0);
  Serial.begin(9600);
  throttleServo.attach(servoPin);
}

// the loop routine runs over and over again forever:
void loop() {
  if (digitalRead(enableSwitch))
    {
      digitalWrite(enableRelay,1);
      enabled=true;
    }
  else
    {
      digitalWrite(enableRelay,0);
      enabled=false;
    }
    
  if (enabled) 
    {
      throttleServo.write(map(analogRead(A0),180,870,50,110));
      //throttleServo.write(90);
      lBEnabled=digitalRead(lBSwitch);
      rBEnabled=digitalRead(rBSwitch);
    }
  else
    {
      throttleServo.write(50);
      lBEnabled=0;
      rBEnabled=0; 
    }
  
if ((lBEnabled == 1) or (rBEnabled == 1)) {
   if (!timerSet) {
    startTimer=millis();
    timerSet=true;
   }
   currentTimer=millis(); 
   if (currentTimer < startTimer + 400)
   {
    if (lBEnabled == 1) digitalWrite(lBlinker,1);
    if (rBEnabled == 1) digitalWrite(rBlinker,1);
    currentTimer=millis(); 
   }
   else
   {
    digitalWrite(lBlinker,0);
    digitalWrite(rBlinker,0);
   }
    if (currentTimer >=  startTimer + 800) startTimer=millis();  
  }
  else
  {
    digitalWrite(lBlinker,0);
    digitalWrite(rBlinker,0);
  }
 
}
  
