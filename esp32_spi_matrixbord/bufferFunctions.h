void showBuffer(){
  digitalWrite (clkPin, 0);
  SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
  for (int y=numRow; y >=0 ; y--){      // for every row in the character, 0..6 rows of leds
    for (int x=(numRegister*numDisplay); x >= 0 ; x--){    // for byte on the line buffer, 0..12 shift registers of 8 bit
      SPI.transfer(rowBuffer[x][y]);
    }
    digitalWrite (rowPins[y], 0);
    digitalWrite (oePin, 0);
    delayMicroseconds(300);
    digitalWrite (oePin, 1);
    digitalWrite (rowPins[y], 1);
  }
SPI.endTransaction();
}

void clearDisplay(int displayNumber){
  for (int x=0; x < numChar ; x++){
    for (int y=0; y < numRow ; y++){
      rowBuffer[(x + ((displayNumber-1) * numRegister))][y] = B00000000;
    }
  }
}

void writeDisplay(int displayNumber, String text){
  for (int x=0; x <= (text.length()) ; x++){
    for (int y=0; y < numRow ; y++){
      if ((isAscii(text[x])) and (!(text[x] == 0))){
        rowBuffer[(x + ((displayNumber-1) * numRegister))][y] = charset[(int(text[x])-32)][y];
      }
    }
  }
}
