/*
  Shift Register Example
 for 74HC595 shift register

 This sketch turns reads serial input and uses it to set the pins
 of a 74HC595 shift register.

 Hardware:
 * 74HC595 shift register attached to pins 8, 12, and 11 of the Arduino,
 as detailed below.
 * LEDs attached to each of the outputs of the shift register.


 Created 22 May 2009
 Created 23 Mar 2010
 by Tom Igoe

 */

//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 20;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 18;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = 19;
const int row1 = 8;
const int row2 = 9;
byte leds = 0;

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  
  pinMode(row1, OUTPUT);
  pinMode(row2, OUTPUT);

  digitalWrite(row1,HIGH);
  digitalWrite(row2,HIGH);
  
  Serial.begin(9600);
  Serial.println("reset");
}

void loop() 
{
  leds = 0;
  updateShiftRegister();
  for (int i = 0; i < 8; i++)
  {
    bitSet(leds, i);
    updateShiftRegister();
    delay(100);
  }
}
 
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}
