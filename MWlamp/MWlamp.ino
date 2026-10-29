#include "FastLED.h"
#define NUM_LEDS 2
#define DATA_PIN 7

String LEDCOLORS[7] = {"Red", "Orange", "Yellow", "Green", "Blue", "HotPink", "White"}; 
CRGB leds[NUM_LEDS];

void setup() {
       FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
   for(int colorLed = 0; colorLed < NUM_LEDS; colorLed = colorLed + 1)	{
      leds[colorLed] = CRGB::LEDCOLOR[random(7)];
      FastLED.show();
      delay(300);
      leds[colorLed] = CRGB::LEDCOLOR[random(7)];
   }
}
