const byte rowPins[7] = {16, 17, 18, 19, 21, 22, 23}; // row latchpins, 0=enabled, 1=disabled
#define clkPin 25     // Clock signal for 5821
#define oePin 26      // Not Output Enabled for 5821, 0=enabled, 1=disabled
#define dataPin 27    // Data In 5821
#define numCols 96    // 96 positions (95 leds) horizontally
#define numRows 7     // 7 rows of leds vertically
#define numChar 19    // 12 characters of 8 bytes wide per line
byte rowBuffer[numChar][numRows];

void setup() {
  for(byte i = 0; i < numRows; i++){
    pinMode(rowPins[i], OUTPUT);
  }
  for(byte i = 0; i < numRows; i++){
    digitalWrite(rowPins[i],1);
  }
  pinMode (clkPin, OUTPUT);
  pinMode (oePin, OUTPUT);
  pinMode (dataPin, OUTPUT);
  digitalWrite (clkPin, 0);
  digitalWrite (oePin, 1);
  digitalWrite (dataPin, 0);
  Serial.begin(9600);
}

void loop() {
  clearDisplay();
  fillDisplay();
  }

void showRow(int rowNumber){
    digitalWrite (oePin, 0);
    digitalWrite (rowPins[rowNumber], 0);
    delay(100);
    digitalWrite (rowPins[rowNumber], 1);
    digitalWrite (oePin, 1);
}

void clearDisplay(){
  for (int x=0; x < numChar ; x++){
    for (int y=0; y < numRows ; y++){
      rowBuffer[x][y] = B00000;
    }
  }
  showBuffer();
}

void fillDisplay(){
  for (int x=0; x < numChar ; x++){
    for (int y=0; y < numRows ; y++){
      rowBuffer[x][y] = B11111;
    }
   showBuffer();
  }
}


void showBuffer(){
    for (int y=0; y < numRows ; y++){      // for every row in the character, 1..7
     for (int x=0; x < numChar ; x++){     // for every character on the line, 12..1
       digitalWrite (clkPin, 0);
       shiftOut(dataPin,clkPin,LSBFIRST,rowBuffer[x][y]);
       //for (int z=0; z < 5 ; z++){          // for every bit in the byte, 8..1
       //  digitalWrite(dataPin,bitRead(rowBuffer[x][y],z ));
       //  delayMicroseconds(1);
       //  digitalWrite (clkPin, 1);
       //  delayMicroseconds(1);
       //  digitalWrite (clkPin, 0);
      //}
    }
    showRow(y);
  }
}
