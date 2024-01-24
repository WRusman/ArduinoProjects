#include <SPI.h>
#include "charset.h";
#include "defines.h";
#include "bufferFunctions.h";
#include "displayThreads.h";
#include "WiFiManager.h" 
#include "globalFunctions.h"

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
  
  SPI.begin(18,19,23,5); // VSPI Interface
  
  
  wifiSetup();
  Serial.println("Connected to WiFi");    

  xTaskCreate(
    outputThread
    ,  "outputThread"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  (void*)&rowBuffer
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
 
  xTaskCreate(
    display1Thread
    ,  "display1Thread"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  (void*)&rowBuffer
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  
  xTaskCreate(
    display2Thread
    ,  "display2Thread"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  
  xTaskCreate(
    display3Thread
    ,  "display3Thread"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  
  xTaskCreate(
    display4Thread
    ,  "display4Thread"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  
  xTaskCreate(
    display5Thread
    ,  "display5Thread"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  
  xTaskCreate(
    display6Thread
    ,  "display6Thread"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  
  xTaskCreate(
    display7Thread
    ,  "display7Thread"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
  
}


void loop() {

}
