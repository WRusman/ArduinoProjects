#include <ESP8266WiFi.h>          
#include <Timezone.h>             //https://github.com/JChristensen/Timezone
#include <NTPClient.h>            //https://github.com/arduino-libraries/NTPClient
#include <WiFiManager.h>
#include <Ticker.h>

int strobePin = 13;  // D7 on the wemos D1 mini
int dataPin =  4;    // D2 on the wemos D1 mini

int clockPin = 0;   // D3 on the wemos D1 mini
int setupPin = 15; // D8 on the wemos D1 mini
// BUILTIN_LED = 2 // D4 on the wemos D1 mini, LOW for ON

int nowminutes, nowhours, buttonState,lastButtonState = LOW ;

//     
//  2--
// 1|  |3
//  5--
// 6|  |4
//  7--
// bit 8 = not used

//0 = 11110110 = 246
//1 = 00110000 = 48
//2 = 01101110 = 110
//3 = 01111010 = 122
//4 = 10111000 = 184
//5 = 11011010 = 218
//6 = 11011110 = 222
//7 = 01110000 = 112
//8 = 11111110 = 254
//9 = 11111010 = 250

byte segChar[] = { 246, 48, 110, 122, 184, 218, 222, 112, 254, 250, 127, 0};

char countdowndate[16], fscountdowndate[16];
unsigned long toTime, lastDebounceTime = 0, debounceDelay = 3;

Ticker ticker;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  
  pinMode(setupPin, INPUT);
    
  digitalWrite(strobePin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  digitalWrite(strobePin, HIGH);

  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);

  ticker.attach(0.6, tick);
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect("klok")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
  timeClient.begin();
}

void loop() {
  checkButton();
  timeClient.update();  
  TimeChangeRule nlDST = {"DST", Last, Sun, Mar, 1, +120};  //UTC + 2 hours
  TimeChangeRule nlSTD = {"STD", Last, Sun, Oct, 1, +60};   //UTC + 1 hour
  Timezone MyTZ(nlDST, nlSTD);
  time_t local_time = MyTZ.toLocal(timeClient.getEpochTime());
  printDateTime(local_time);
   
  delay(1000);
}


// format and print a time_t value.
void printDateTime(time_t t)
{
    nowminutes=minute(t);
    nowhours=hour(t);

    Serial.print(nowhours);
    Serial.print(":");
    Serial.println(nowminutes);

    digitalWrite(strobePin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[nowminutes % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(nowminutes / 10) % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[nowhours % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(nowhours / 10) % 10]);  
    digitalWrite(strobePin, HIGH);
}

void tick()
{
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void configModeCallback (WiFiManager *myWiFiManager) {
  digitalWrite(strobePin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);   
  shiftOut(dataPin, clockPin, MSBFIRST, 0);  
  shiftOut(dataPin, clockPin, MSBFIRST, 0);  
  shiftOut(dataPin, clockPin, MSBFIRST, 0);  
  digitalWrite(strobePin, HIGH);
  
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

void checkButton(){
  buttonState = digitalRead(setupPin);
  if (buttonState == 1){
    if (buttonState == lastButtonState) {
      lastDebounceTime += 1;
    }
    if (lastDebounceTime >= debounceDelay) {
     configPortal();
     lastDebounceTime = 0;
    }
  }
  else {
    lastDebounceTime = 0;
  }
  lastButtonState = buttonState;
}

void configPortal(){
  ticker.attach(0.6, tick);
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  if (!wm.startConfigPortal("klok")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
  ESP.reset();
}  
