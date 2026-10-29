#include <Adafruit_NeoPixel.h>
#define PIN 5             // pin 5 (wemos D1) is connected to the data pin of the led strip
#define NUM_LEDS 144      // the total number of leds 
#define NUM_SPARKLE 26    // the number of sparkling led's in the chain
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800); 

// the array of led's that will be red and sparkle at random
int leds[NUM_SPARKLE] = {0,4,10,16,22,27,33,37,44,50,58,63,70,74,79,85,90,97,102,107,114,120,126,132,138,143} ;

void setup(){
  strip.begin();          
  
  // set all the led's on the strip dimmed green
  for(int i = 0; i < NUM_LEDS; i++ ) {
    strip.setPixelColor(i,0,0x05,0);
  }
  strip.show();

   // set the led's in the sparkle array dimmed red  
   for (int l=0; l<= NUM_SPARKLE; l++) {
     strip.setPixelColor(leds[l],0x10,0x00,0x00);
   }
   strip.show();
}

void loop() {
  sparkle(leds[random(NUM_SPARKLE)]);  // sparkle a random led from the array of red led's
}

void sparkle (int sparkleLed) {
  // intensify the brightness fast
  for (int s=10; s<= 125; s++)   
  {
    strip.setPixelColor(sparkleLed,s*2,0,0);
    strip.show();
  }
  // and dim the brightness fast 
  for (int s=125; s>= 10; s--)
  {
    strip.setPixelColor(sparkleLed,s*2,0,0);
    strip.show();
  }
}
