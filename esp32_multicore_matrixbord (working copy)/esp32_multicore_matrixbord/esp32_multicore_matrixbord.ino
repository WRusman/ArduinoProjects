TaskHandle_t showBuffer;

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "charset.h";

const byte rowPins[7] = {16, 17, 18, 19, 21, 22, 23}; // row latchpins, 0=enabled, 1=disabled
#define clkPin 25     // Clock signal for 5821
#define oePin 26      // Not Output Enabled for 5821, 0=enabled, 1=disabled
#define dataPin 27    // Data In 5821
#define numCols 96    // 96 positions (95 leds) horizontally
#define numRows 7     // 7 rows of leds vertically
#define numChar 19    // 19 characters of 8 bytes wide per line
byte rowBuffer[numChar][numRows];

String message = "0000000000000000000"; 

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
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
                    showBufferCode,   /* Task function. */
                    "showBuffer",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &showBuffer,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500);
}

void showBufferCode( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  while(true){
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0; 
    digitalWrite (clkPin, 0);
      for (int y=numRows; y >=0 ; y--){      // for every row in the character, 0..6
        for (int x=numChar; x >=0 ; x--){    // for every character on the line, 0..18
           for (int z=0; z < 5 ; z++){       // for bit in this character on the line, 0..4
             digitalWrite(dataPin,bitRead(rowBuffer[x][y],z ));
             delayMicroseconds(1);
             digitalWrite (clkPin, 1);
             delayMicroseconds(1);
             digitalWrite (clkPin, 0);
            }  
        }
      digitalWrite (rowPins[y], 0);
      digitalWrite (oePin, 0);
      delay(1);
      //delayMicroseconds(800);
      digitalWrite (rowPins[y], 1);
      digitalWrite (oePin, 1);
    }
  }
}

void loop() {
    writeMessageString();
    delay(1000);
    clearDisplay();
    delay(1000);
}

void clearDisplay(){
  for (int x=0; x < numChar ; x++){
    for (int y=0; y < numRows ; y++){
      rowBuffer[x][y] = B00000;
    }
  }
}

void writeMessageString(){
  //message.trim();
  for (int x=0; x <= (message.length()) ; x++){
    for (int y=0; y < numRows ; y++){
      if ((isAscii(message[x])) and (!(message[x] == 0))){
        rowBuffer[x][y]= charset[(int(message[x])-32)][y];
      }
    }
  }
}
