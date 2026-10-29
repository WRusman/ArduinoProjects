int OEPin = 7;     // D7 on the arduino Nano
int clearPin = 8;  // D8 on the arduino Nano
int latchPin = 9;  // D9 on the arduino Nano
int dataPin = 11;  // D11 on the arduino Nano
int clockPin = 13; // D13 on the arduino Nano

//     
//  -8-
// 3|  |7
//  -2-
// 4|  |6
//  -5-
//
// bit 1 = not used
//
//0 = 00111111 = 63
//1 = 00000110 = 6
//2 = 01011011 = 91
//3 = 01001111 = 79
//4 = 01100110 = 102
//5 = 01101101 = 109
//6 = 01111101 = 125
//7 = 00000111 = 7
//8 = 01111111 = 127
//9 = 01101111 = 111

byte segChar[] = { 63, 6, 91, 79, 102, 109, 125, 7, 127, 111};

void setup() {
  pinMode(latchPin, OUTPUT);      // set the pin connected to the latch as type OUTPUT
  pinMode(clockPin, OUTPUT);      // set the pin connected to the clock as type OUTPUT
  pinMode(dataPin, OUTPUT);       // set the pin connected to the serial data as type OUTPUT
  pinMode(OEPin, OUTPUT);         // set the pin connected to the Output Enabled as type OUTPUT
  pinMode(clearPin, OUTPUT);      // set the pin connected to the clear as type OUTPUT
  digitalWrite(latchPin, LOW);    // set the pin connected to the latch low for disabled
  digitalWrite(clearPin, HIGH);   // set the pin connected to the clear low for disabled (inverted)
  digitalWrite(OEPin, LOW);       // set the pin connected to the OE low for enabled (inverted)
}

void loop() {
  // for 0 to 9
for (int x=0; x<10; x++){
  // shift out the value associated with the x'th value in the array segChar to the third digit the display
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[x]); 
  // shift out the value associated with the x'th value in the array segChar to the display, 
  // shoving the previous character to the second digit
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[9-x]); 
  // shift out the value associated with the x'th value in the array segChar to the display, 
  // shoving the previous character to the second digit and the first character to the first digit
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[0]); 

  digitalWrite(OEPin, HIGH);    // turn the display off when updating (inverted)
  digitalWrite(latchPin, HIGH); // copy the data from the buffer to the output -
  digitalWrite(latchPin, LOW);  // with a clockpulse on the latch
  digitalWrite(OEPin, LOW);     // turn the display on again (inverted)
  
  delay (1000);                 // update the display every second
  }
}
