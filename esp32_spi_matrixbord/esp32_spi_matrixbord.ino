#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "Ticker.h";
#include <SPI.h>
#include "charset.h";
#include "defines.h";
#include "bufferFunctions.h";
#include "displayThreads.h";
#include "OTAWrapper.h"
#include "WiFiManager.h" 
#include "globalFunctions.h"

TaskHandle_t displayThread;
TaskHandle_t mainThread;

void display1Thread();
void display2Thread();
void display3Thread();
void display4Thread();
void display5Thread();
void display6Thread();
void display7Thread();
void display8Thread();
void blinkLedThread();

Ticker timer1(display1Thread,1000); // every 1000ms
Ticker timer2(display2Thread,1000); // every 1000ms
Ticker timer3(display3Thread,1000); // every 1000ms
Ticker timer4(display4Thread,1000); // every 1000ms
Ticker timer5(display5Thread,1000); // every 1000ms
Ticker timer6(display6Thread,1000); // every 1000ms
Ticker timer7(display7Thread,1000); // every 1000ms
Ticker timer8(display8Thread,1000); // every 1000ms
Ticker timer9(blinkLedThread,1000); // every second
Ticker timer10(OTAThread,500); // every half second



void wifiSetup() {
  wifiManager.setAPCallback([] (WiFiManager * myWiFiManager) {
    Serial.println("Starting AP");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
  });
  wifiManager.setConfigPortalTimeout(10* 60);
  if (!wifiManager.autoConnect(AP_NAME)) {
    ESP.restart();
    delay(1000);
  }
}


void setup() {
  for(byte i = 0; i < numRow; i++){
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i],1);
  }
  pinMode (clkPin, OUTPUT);digitalWrite (clkPin, 0);
  pinMode (oePin, OUTPUT);digitalWrite (oePin, 1);
  pinMode (dataPin, OUTPUT);digitalWrite (dataPin, 0);
  pinMode (LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial) {
    delay(20);
  }
  
  SPI.begin(18,19,23,5);
  wifiSetup();
  Serial.println("Connected to WiFi");    
  ota.setup(OTA_NAME, OTA_PASSWD);
  
  ota.onStart([] () {
    Serial.println("OTA START");
  });
  ota.onEnd([] () {
    Serial.println(".");
    Serial.println("OTA END");
  });
  ota.onError([] () {
    Serial.println("OTA ERROR");
  });
   ota.onProgress([] () {
     int p = ota.getProgress();
     if (p % 5 == 0) {
       Serial.print(".");
     }
  });
 
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
  timer7.start();
  timer8.start();
  timer9.start();
  timer10.start();

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
    timer7.update();
    timer8.update();
    timer9.update();
    timer10.update();
  }
}  
