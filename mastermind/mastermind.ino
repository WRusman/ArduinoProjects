#include <Servo.h>
#include <FastLED.h>
#include <TM1637Display.h>
#define CLK 11
#define DIO 12

const byte buttonPins[]={2,3,4,5};
const byte submitPin=6;
const byte servoPin=A3;
const byte keepAlivePin=7;
const byte codeLength=4;
const byte numSymbols=10;
const byte numLeds=4;
uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };


bool lastButtonState[codeLength];
byte secret[codeLength] = {0,0,0,0};
byte guess[codeLength] = {0,0,0,0};
int numGuesses = 0;
CRGB leds[numLeds];
Servo lockServo;

TM1637Display display(CLK, DIO);

void calculateResponse(byte &correctCount, byte &falseCount){
  bool secretUsed[codeLength]={false,}; // false, false, false, false
  bool guessUsed[codeLength]={false,}; // false, false, false, false

  correctCount = 0;
  for(int i=0; i<codeLength; i++){
    if (guess[i] == secret[i]) {
      correctCount++;
      secretUsed[i]=true;
      guessUsed[i]=true;
    }
  }

  falseCount=0;
  for(int i=0; i<codeLength; i++){
    if(!guessUsed[i]){
      for(int j=0; j<codeLength; j++){
        if (!secretUsed[j] && (guess[i] == secret[j])) {
          falseCount++;
          secretUsed[j]=true;
          break;
        }
      }
    }
  }
  return (correctCount == codeLength);
}




void displayResponse(int numCorrect, int numFalse){
  for(int i=0; i<numLeds; i++){
    leds[i] = CRGB::Red;
  }
  for (int loop=1; loop <7; loop++){
    for (int i=0; i<numCorrect*(numLeds/4); i++){
      leds[i] = CRGB::Green;
    }
    for (int i=numCorrect*(numLeds/4);i<(numCorrect+numFalse)*(numLeds/4); i++){
      leds[i] = (loop%2) ? CRGB::Black : CRGB::Orange;
    }
    FastLED.show();
    delay(400);
  }
  for(int i=0; i<numLeds; i++){
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void getInput() {
  while (!digitalRead(submitPin)){
    int state = digitalRead(LED_BUILTIN);  
    digitalWrite(LED_BUILTIN, !state);     
    digitalWrite(keepAlivePin, !state); // keep usb powerbank active    
    delay(100);
  }
  while (digitalRead(submitPin)){
    int state = digitalRead(LED_BUILTIN);  
    digitalWrite(LED_BUILTIN, !state);     
    digitalWrite(keepAlivePin, !state);     

    for (int i=0; i<4; i++){
      bool buttonState = digitalRead(buttonPins[i]);
      if(!buttonState && lastButtonState[i]) {
        guess[i]++;
        if(guess[i]>=numSymbols) {guess[i]=0; }
      }
      lastButtonState[i]=buttonState;
      //display the new guess code
      data[0] = display.encodeDigit(guess[0]);
      data[1] = display.encodeDigit(guess[1]);
      data[2] = display.encodeDigit(guess[2]);
      data[3] = display.encodeDigit(guess[3]);
      display.setSegments(data);
    }
  }  
}

void setup() {
  for (int i=0; i <4; i++){
    pinMode(buttonPins[i],INPUT_PULLUP);
  }
  randomSeed(analogRead(A5));
  secret[0] = random(9);
  secret[1] = random(9);  
  secret[2] = random(9);
  secret[3] = random(9);  
  pinMode(submitPin,INPUT_PULLUP);
  pinMode(servoPin,OUTPUT);
  pinMode(A2,OUTPUT);
  lockServo.attach(servoPin);
  FastLED.addLeds<WS2812B, A2, RGB>(leds, numLeds);
  FastLED.setBrightness(80);
  display.setBrightness(0x0a);
  display.clear();
  lockServo.write(85);
  guess[0] = 0; 
  guess[1] = 0;
  guess[2] = 0; 
  guess[3] = 0; 
}

void loop() {
  getInput();
  byte falseCount=0, correctCount=0;
  calculateResponse(correctCount, falseCount);

  if(correctCount == codeLength) {
    //code correct
    lockServo.write(55);

    for(int i=0; i<numLeds; i++){
      leds[i] = CRGB::Green;
      FastLED.show();
      delay(200);
    }
    for(int i=0; i<numLeds; i++){
      leds[i] = CRGB::Black;
      FastLED.show();
      delay(20);
    }
  }
  //code incorrect
  else 
  {
  displayResponse(correctCount, falseCount);
  }
  numGuesses++;
}
