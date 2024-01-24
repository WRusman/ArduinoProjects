#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(8, 0, NEO_GRB + NEO_KHZ800);

void setup() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  pinMode(0,OUTPUT);
  pixels.begin();
  pixels.clear();
}

void loop() {
for (int x=-1; x < 8 ; x++)
{
  pixels.clear();
  pixels.setPixelColor(x-1, pixels.Color(255, 0, 0));
  pixels.setPixelColor(x, pixels.Color(255, 0, 0));
  pixels.setPixelColor(x+1, pixels.Color(255, 0, 0));
  pixels.show();
  if ((x == -1) or (x == 8)) delay(200);
  if ((x == 0) or (x == 7)) delay(100);
  if ((x == 1) or (x == 6)) delay(100);
  else delay(50);
}

for (int x=8; x > -1; x--)
{
  pixels.clear();
  pixels.setPixelColor(x-1, pixels.Color(255, 0, 0));
  pixels.setPixelColor(x, pixels.Color(255, 0, 0));
  pixels.setPixelColor(x+1, pixels.Color(255, 0, 0));
  pixels.show();
  if ((x == -1) or (x == 8)) delay(200);
  if ((x == 0) or (x == 7)) delay(100);
  if ((x == 1) or (x == 6)) delay(100);
  else delay(50);
}  
}
