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
