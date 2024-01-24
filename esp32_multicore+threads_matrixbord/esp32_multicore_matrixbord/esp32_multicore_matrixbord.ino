#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "Ticker.h";
#include "charset.h";
#include "defines.h";
#include "bufferFunctions.h";
#include "displayThreads.h";

TaskHandle_t displayThread;
TaskHandle_t mainThread;

void display1Thread();
void display2Thread();
void display3Thread();
void display4Thread();
void display5Thread();
void blinkLedThread();

Ticker timer1(display1Thread,100); // every 100ms
Ticker timer2(display2Thread,100); // every 100ms
Ticker timer3(display3Thread,100); // every 100ms
Ticker timer4(display4Thread,100); // every 100ms
Ticker timer5(display5Thread,100); // every 100ms
Ticker timer6(blinkLedThread,500); // every half second


void setup() {
  for(byte i = 0; i < numRows; i++){
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i],1);
  }
  pinMode (clkPin, OUTPUT);digitalWrite (clkPin, 0);
  pinMode (oePin, OUTPUT);digitalWrite (oePin, 1);
  pinMode (dataPin, OUTPUT);digitalWrite (dataPin, 0);
  pinMode (LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  delay(500);
 
  xTaskCreatePinnedToCore(
                    displayThreadCode,   /* Task function. */
                    "displayThread",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &displayThread,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(200);

  xTaskCreatePinnedToCore(
                    mainThreadCode,   /* Task function. */
                    "mainThread",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &mainThread,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */                  
  delay(200);
  
  timer1.start();
  timer2.start();
  timer3.start();
  timer4.start();  
  timer5.start();
  timer6.start();

}

void loop() {
}

void displayThreadCode( void * pvParameters ){
  Serial.print("displayThread running on core ");
  Serial.println(xPortGetCoreID());
  while(true){
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;                          // feed watchdog timer of core 0 to prevent reboots
    TIMERG0.wdt_wprotect=0; 
    showBuffer();
  }
}  

void mainThreadCode( void * pvParameters ){
  Serial.print("mainThread running on core ");
  Serial.println(xPortGetCoreID());
  while(true){
    timer1.update();
    timer2.update();
    timer3.update();
    timer4.update();
    timer5.update();
    timer6.update();
  }
}  
