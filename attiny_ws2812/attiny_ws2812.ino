#include "FastLED.h"
#define NUM_LEDS 90
#define DATA_PIN 3
#define BRIGHTNESS 12
#define NUM_COLORS 3

CRGB leds[NUM_LEDS];
int j = 0;        // Starting color number
CRGB myColors[] = {CRGB::Red, CRGB::Green, CRGB::Orange};

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear ();
}

void loop() {
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = myColors[(i + j) % NUM_COLORS];  // mod restricts the color number
  }
  FastLED.show();

  j = (j + 1) % NUM_COLORS;                // Cycle through the starting LED#
  delay(500);
}