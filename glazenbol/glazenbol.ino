#include "FastLED.h"
#define NUM_LEDS 68
#define DATA_PIN 13

// Global variables can be changed on the fly.
uint8_t max_bright = 60;                                      // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

uint8_t frequency = 50;                                       // controls the interval between strikes
uint8_t flashes = 8;                                          //the upper limit of flashes per strike
unsigned int dimmer = 1;

uint8_t ledstart;                                             // Starting location of a flash
uint8_t ledlen;       

long firstval = 0xff00ff;

void setup() { 
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
     FastLED.setBrightness(max_bright);
}
   
 void loop () {

  int loopcounter=0;
while (loopcounter < 30){   
  ledstart = random8(NUM_LEDS);           // Determine starting location of flash
  ledlen = random8(NUM_LEDS-ledstart);    // Determine length of flash (not to go beyond NUM_LEDS-1)
  for (int flashCounter = 0; flashCounter < random8(3,flashes); flashCounter++) {
    if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
    else dimmer = random8(1,3);           // return strokes are brighter than the leader
    fill_solid(leds+ledstart,ledlen,CHSV(255, 0, 255/dimmer));
    FastLED.show();                       // Show a section of LED's
    delay(random8(4,10));                 // each flash only lasts 4-10 milliseconds
    fill_solid(leds+ledstart,ledlen,CHSV(255,0,0));   // Clear the section of LED's
    FastLED.show();     
    if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
    delay(50+random8(100));               // shorter delay between strokes  
    loopcounter++;
  } // for()
  delay(random8(frequency)*10);          // delay between strikes
}
   // fade in 15 seconden naar felwit
   fill_solid (leds, NUM_LEDS, CRGB(255,255,255));  

 for (int i = 0; i < 60; i++){
     FastLED.setBrightness(i);
   delay(120); 
   FastLED.show(); 
   }  


// fade naar rood/groen
 for (int i = 255; i >0; i--){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(255,i,i),CRGB(i,255,i));  
   delay(5); 
   FastLED.show(); 
   }  
   
delay (900);

 for (int i = 0; i < 255; i++){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(255,i,i),CRGB(i,255,i));  
   FastLED.show(); 
   }  

// fade naar groen/blauw
 for (int i = 255; i >0; i--){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(i,255,i),CRGB(i,i,255));  
   delay(1); 
   FastLED.show(); 
   }  

delay(900);

 for (int i = 0; i < 255; i++){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(i,255,i),CRGB(i,i,255));  
   FastLED.show(); 
   }  

// fade naar blauw/rood
 for (int i = 255; i >0; i--){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(i,i,255),CRGB(i,i,255),CRGB(255,i,i));  
   delay(5); 
   FastLED.show(); 
   }  

delay(900);

 for (int i = 0; i < 255; i++){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(i,i,255),CRGB(i,i,255),CRGB(255,i,i));  
   FastLED.show(); 
   }
   
   
// fade naar blauw/groen
 for (int i = 255; i >0; i--){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(i,i,255),CRGB(i,255,i));  
   delay(1); 
   FastLED.show(); 
   }  

delay(900);

 for (int i = 0; i < 255; i++){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(i,i,255),CRGB(i,255,i));  
   delay(1); 
   FastLED.show(); 
   }  

// fade naar rood
 for (int i = 255; i >0; i--){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(255,i,i),CRGB(255,i,i));  
   delay(1); 
   FastLED.show(); 
   }  

delay(2500);

 for (int i = 0; i < 255; i++){
   fill_gradient_RGB (leds, NUM_LEDS, CRGB(255,i,i),CRGB(255,i,i));  
   FastLED.show(); 
   }  

// fade naar wit/blauw/groen/groen
 for (int i = 255; i >0; i--){
   fill_gradient_RGB (leds, NUM_LEDS,CRGB(255-i,255-i,255-i),CRGB(i,i,120),CRGB(i,120,i),CRGB(i,120,i));  
   delay(1); 
   FastLED.show(); 
   }  

delay(2500);

 for (int i = 0; i <120; i++){
   fill_gradient_RGB (leds, NUM_LEDS,CRGB(255,255,255),CRGB(i,i,120),CRGB(i,120,i),CRGB(i,120,i));  
   FastLED.show(); 
   }  
 for (int i = 120; i <255; i++){
   fill_gradient_RGB (leds, NUM_LEDS,CRGB(255,255,255),CRGB(i,i,i),CRGB(i,i,i),CRGB(i,i,i));  
   FastLED.show(); 
   }  
 
 for (int i = 255; i >= 0; i--){
   fill_solid (leds, NUM_LEDS, CRGB(i,i,i));
   delay(30);
   FastLED.show(); 
   }  
   
 while(1) ;  
//delay(3000);




 } //loop
 
