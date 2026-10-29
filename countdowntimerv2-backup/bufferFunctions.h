void showBuffer(){
  for (int displayRow=numRow; displayRow >=0 ; displayRow--){       // for every row on the display, 7..1
    for (int displayReg=numReg; displayReg >= 0 ; displayReg--){    // for every register in the buffer, 36..1
      shiftOut(dataPin, clkPin, LSBFIRST, displayBuffer[displayReg][displayRow]);
    }
  digitalWrite (rowPins[displayRow], LOW);
  digitalWrite (oePin, LOW);
  delay(1);
  digitalWrite (rowPins[displayRow], HIGH);
  digitalWrite (oePin, HIGH);
  }
}


void clearBuffer(){
  for (int x=0; x < numReg ; x++){
    for (int y=0; y < numRow ; y++){
      textBuffer[x][y] = B00000000;
      displayBuffer[x][y] = B00000000;
    }
  }
}

void message2Buffer(){
  clearBuffer();
  for (int messageChar=0; messageChar <= (message.length()) ; messageChar++){                     // for every character "messageChar" in message
    for (int displayRow=0; displayRow < numRow ; displayRow++){                                   // for every row in the display (0..6)
      if ((isAscii(message[messageChar])) and (!(message[messageChar] == 0))){                    // if character is ascci and not null
        textBuffer[messageChar][displayRow]= charset[(int(message[messageChar])-32)][displayRow]; // store the 8 bits for the character in array textBuffer[messageChar] for line [displayRow]
        //displayBuffer[messageChar][displayRow]=textBuffer[messageChar][displayRow];
      }
    }
  }

  for (int displayRow=0; displayRow < numRow ; displayRow++){                                     // for every row in the display (0..6)
    int displayBufferByte=0;                                                                      // start at the beginning of the displayBuffer
    uint8_t currentByte = 0;                                                                      // start at the beginning of the textBuffer
    int bitPos=0;                                                                                 // start at bit 0 of the first displayBuffer byte
    for (int textBufferChar=0; textBufferChar < numChar; textBufferChar++){                       // for every byte "bufferChar" in textBuffer

      for (int charBit = 3; charBit < 8; charBit++) {                                             // for every bit 3..8 from the Byte
        uint8_t currentBit = (textBuffer[textBufferChar][displayRow] >> (7 - charBit)) & 0x01;    // shift to the correct position and mask                           
        currentByte |= (currentBit << (7 - bitPos));                                              // add the bit to the current working Byte    
        bitPos++;                                                                                 // go to the next bit of the textBuffer Byte
        if (bitPos == 8) {                                                                        // if we are over bit 8 of the working byte  
          displayBuffer[displayBufferByte++][displayRow] = currentByte;                           // switch to the next display buffer byte
          currentByte = 0;                                                                        // clear the content of the working byte
          bitPos = 0;                                                                             // and start at the beginning of the working byte
        }
        if (bitPos > 0) {                                                                         // when finished and the current bit counter is more than zero
          displayBuffer[displayBufferByte][displayRow] = currentByte;                             // write the remaining bits to the displayBuffer
        }
      }
      
    }  
  }
}
