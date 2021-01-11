#include <Adafruit_NeoPixel.h>
#define PIN 0
#define NUM_LEDS 144
#define NUM_SPARKLE 26
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int leds[NUM_SPARKLE] = {0,4,10,16,22,27,33,37,44,50,58,63,70,74,79,85,90,97,102,107,114,120,126,132,138,143} ;

void setup(){
  strip.begin();
  setAll(0,0x05,0);

   for (int l=0; l<= NUM_SPARKLE; l++)
  {
   strip.setPixelColor(leds[l],0x10,0x00,0x00);
  }
  strip.show();
  }



void loop() {
 sparkle(leds[random(NUM_SPARKLE)]);
}

void sparkle (int sparkleLed) {
   for (int s=10; s<= 125; s++)
  {
   strip.setPixelColor(sparkleLed,s*2,0,0);
   strip.show();
  }
   for (int s=125; s>= 10; s--)
  {
   strip.setPixelColor(sparkleLed,s*2,0,0);
   strip.show();
  }
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
   strip.setPixelColor(i,red,green,blue);
  }
  strip.show();
}
