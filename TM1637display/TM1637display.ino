#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3

TM1637Display display(CLK, DIO);

void setup()
{
      display.setBrightness(3);
}

void loop()
{
  display.showNumberDecEx(128, 0x40, false,3,0);
  delay (2000);

}
