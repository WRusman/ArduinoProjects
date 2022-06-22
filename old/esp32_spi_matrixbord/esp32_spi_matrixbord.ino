#include <SPI.h>

TaskHandle_t showBuffer;
TaskHandle_t Task2;

const byte rowPins[7] = {13, 14, 27, 26, 25, 33, 32};   // row latchpins, 0=enabled, 1=disabled
#define clkPin 18     // Clock signal for 5821
#define oePin 17      // Not Output Enabled for 5821, 0=enabled, 1=disabled
#define dataPin 23    // Data In 5821
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

  SPI.begin();
  SPI.beginTransaction(SPISettings(800000, MSBFIRST, SPI_MODE3));
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
  xTaskCreatePinnedToCore(
                    Task2Code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 0 */                  
  delay(500);

}

void showBufferCode( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  while(true){
    digitalWrite (clkPin, 0);
      for (int y=0; y < numRows ; y++){      // for every row in the character, 1..7
        for (int x=0; x < numChar ; x++){     // for every character on the line, 12..1
          for (int z=0; z < 5 ; z++){          // for every bit in the byte, 8..1
            SPI.transfer(bitRead(rowBuffer[x][y],z));
          }
        }  
      digitalWrite (rowPins[y], 0);
      digitalWrite (oePin, 0);
      delay(10);
      digitalWrite (rowPins[y], 1);
      digitalWrite (oePin, 1);
    }
  }
}

void Task2Code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  while(true){
    fillDisplay();
    delay(5000);
    clearDisplay();
    delay(5000);
    Serial.println("loop done");
  }
}

void loop() {
}

void clearDisplay(){
  for (int x=0; x < numChar ; x++){
    for (int y=0; y < numRows ; y++){
      rowBuffer[x][y] = B00000;
    }
  }
}

void fillDisplay(){
  for (int x=0; x < numChar ; x++){
    for (int y=0; y < numRows ; y++){
      rowBuffer[x][y] = B11111;
    }
  }
}
