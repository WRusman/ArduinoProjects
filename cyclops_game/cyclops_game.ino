#include <FastLED.h>

#define numLeds 50 
#define dataPin 7 
#define buttonPin 5
#define brightness 100

CRGB leds[numLeds];

int buttonState, orbLocation, cyclopsLocation=0, level=1;
bool dirReversed=false, newGame=true;


void setup() 
{
  FastLED.addLeds<WS2811,dataPin,RGB>(leds, numLeds).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  
}

void loop() 
{
  checkLocation();
  drawGame();
}
  
void checkLocation()
{
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) //button pressed
  {
    delay(100);
    if (cyclopsLocation != orbLocation)
    {
      flash();
      dirReversed = !dirReversed;
    }
    else
    {
      cylon();
      newGame = true;   
      if (level < 12)
      {
        level++;
      }  
      else
      {
        cylon();
        cylon();
        cylon();
        cylon();
        level=1;
      }
    }
  }
}

void drawGame()
{
  fill_solid(leds, numLeds, CRGB::Black);

  if (newGame) 
  {
    orbLocation=random(numLeds);
    newGame=false;
  }

  if (dirReversed)
    {
      if (cyclopsLocation > 0) cyclopsLocation--;
      else cyclopsLocation=(numLeds-1);
    }
  else
    {
      if (cyclopsLocation < (numLeds-2)) cyclopsLocation++;
      else cyclopsLocation=0;
    }

  leds[orbLocation] = CRGB::Blue;
  leds[cyclopsLocation] = CRGB::Red;

  FastLED.show();  
  switch (level){
    case 1: delay(100);break;
    case 2: delay(90);break;
    case 3: delay(80);break;
    case 4: delay(70);break;
    case 5: delay(60);break;
    case 6: delay(50);break;
    case 7: delay(40);break;
    case 8: delay(30);break;
    case 9: delay(20);break;
    case 10: delay(10);break;
    case 11: delay(7);break;        
    case 12: delay(5);break;
  }
}
  

void fadeall() 
{ 
  for(int i = 0; i < numLeds; i++) { leds[i].nscale8(250); } 
}

void flash() 
{ 
  fill_solid(leds, numLeds, CRGB::Yellow);
  leds[orbLocation] = CRGB::Blue;
  leds[cyclopsLocation] = CRGB::Red;
  FastLED.show();  
//  delay(100);
  for (int x=0; x < 100; x++)
    {
    fadeToBlackBy(leds, numLeds, 4);  
    FastLED.show();
    delay(5);
    }
}


void cylon() 
{ 
  static uint8_t hue = 0;
  for(int i = 0; i < numLeds; i++) {
    leds[i] = CHSV(hue++, 255, 255);
    FastLED.show(); 
    fadeall();
    delay(5);
  }
  for(int i = (numLeds)-1; i >= 0; i--) {
    leds[i] = CHSV(hue++, 255, 255);
    FastLED.show();
    fadeall();
    delay(5);
  }
}
