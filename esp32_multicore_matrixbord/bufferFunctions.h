void showBuffer(){
    digitalWrite (clkPin, 0);
    for (int y=numRows; y >=0 ; y--){      // for every row in the character, 0..6
      for (int x=(numChar*numDisplays); x >=0 ; x--){    // for every character on the line, 0..18
         for (int z=0; z < 5 ; z++){       // for bit in this character on the line, 0..4
           digitalWrite(dataPin,bitRead(rowBuffer[x][y],z ));
         //  delayMicroseconds(1);
           digitalWrite (clkPin, 1);
           digitalWrite (clkPin, 0);
          }  
      }
    digitalWrite (oePin, 0);
    digitalWrite (rowPins[y], 0);
    delay(1);
    digitalWrite (oePin, 1);
    digitalWrite (rowPins[y], 1);
  }

}

void clearDisplay(int displayNumber){
  for (int x=0; x < numChar ; x++){
    for (int y=0; y < numRows ; y++){
      rowBuffer[(x + ((displayNumber-1) * numChar))][y] = B00000;
    }
  }
}

void writeDisplay(int displayNumber, String text){
  for (int x=0; x <= (text.length()) ; x++){
    for (int y=0; y < numRows ; y++){
      if ((isAscii(text[x])) and (!(text[x] == 0))){
        rowBuffer[(x + ((displayNumber-1) * numChar))][y]= charset[(int(text[x])-32)][y];
      }
    }
  }
}
