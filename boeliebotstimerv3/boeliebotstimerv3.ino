#include <TimeLib.h>

int latchPin = 9;  // D9 on the arduino Nano
int dataPin = 11;  // D11 on the arduino Nano
int clockPin = 13; // D13 on the arduino Nano
int resetPin = 2;  // D2 on the arduino Nano
int startPin = 3;  // D3 on the arduino Nano
int OEPin = 7;     // D7 on the arduino Nano
int clearPin = 8;  // D8 on the arduino Nano


int remTime, countdownTime=90, curSecond, prevSecond, startCounter;
bool running, startPressed, resetPressed;
volatile byte readState1 = HIGH;
volatile byte readState2 = HIGH;


byte segChar[] = { 63, 6, 91, 79, 102, 109, 125, 7, 127, 111, 0};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(OEPin, OUTPUT);  
  pinMode(clearPin, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(startPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);
  digitalWrite(latchPin, LOW);
  digitalWrite(clearPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(startPin), startButton, CHANGE);
  attachInterrupt(digitalPinToInterrupt(resetPin), resetButton, CHANGE);
  Serial.begin(115200);
  running = false;
  remTime=countdownTime;
  showDisplay();
}

void loop() {
 if (resetPressed) {
  if (running == false) {
      remTime=countdownTime;
      showDisplay();
      Serial.println("{\"time\":"+String(countdownTime)+"}");
    }
 }

 if (startPressed && (startCounter > 1)) {
  if (running == false) {
      running = true;
      Serial.println("{\"status\":\"start\"}");
    }
    else
    {
      running = false;
      Serial.println("{\"status\":\"stop\"}");
    }
  startCounter = 0;
 }
 
 curSecond = second();
 if (curSecond > prevSecond) {
  startCounter++;
 }

  if (running && remTime > 0) {
       if (curSecond != prevSecond){
        remTime--;
        showDisplay();
        Serial.println("{\"time\":"+String(remTime)+"}"); 
      }
  }
  else if (running && remTime == 0) {
    if (curSecond %2 == 0) {
      showDisplay();
    }
    else {
      clearDisplay();
    }
  }
 prevSecond = curSecond;
 delay(50);
}


void startButton()                               
{
 readState1=digitalRead(startPin);
 if (readState1 == LOW){
  startPressed = true;
  }
  else
  startPressed = false;
}

 
void resetButton()
{
 readState2=digitalRead(resetPin);
 if (readState2 == LOW){
  resetPressed = true;
  }
  else
  resetPressed = false;
}


void showDisplay(){
  int remMinutes=((remTime/60) %10);
  int remSeconds=(remTime-(remMinutes*60));
  digitalWrite(OEPin, HIGH);  
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[remMinutes]);          // digit 1
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[remSeconds /10 %10]);  // digit 2
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[remSeconds %10]);      // digit 3
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
  digitalWrite(OEPin, LOW);
}

void clearDisplay(){
  digitalWrite(OEPin, HIGH);  
  shiftOut(dataPin, clockPin, MSBFIRST, 00000000);       // digit 1
  shiftOut(dataPin, clockPin, MSBFIRST, 00000000);       // digit 2
  shiftOut(dataPin, clockPin, MSBFIRST, 00000000);       // digit 3
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
  digitalWrite(OEPin, LOW);
}
