#include <ArduinoJson.h>
#include <LittleFS.h>
#include <FS.h> 
#include <WiFi.h>          
#include <Timezone.h>             //https://github.com/JChristensen/Timezone
#include <NTPClient.h>            //https://github.com/arduino-libraries/NTPClient
#include <WiFiManager.h>
#include <Ticker.h>

int setupPin = 23; // pin 23 on the devkit v1 for setup mode switch

int calcYear,calcMonth,calcDay,calcHour,calcMinute,calcSecond,toDateDays,toDateHours,toDateMinutes,toDateSeconds,countDownDays,countDownHours,countDownMinutes,countDownSeconds,buttonState,lastButtonState = LOW ;

char countdowndate[16], fscountdowndate[16];
unsigned long toTime, lastDebounceTime = 0, debounceDelay = 3;

Ticker ticker;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
tmElements_t tm;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(setupPin, INPUT);
  
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);

  ticker.attach(0.6, tick);
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect("TalkTimer")) {
    Serial.println("failed to connect and hit timeout");
    ESP.restart();
    delay(1000);
  }
  
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
  timeClient.begin();
  createElements(countdowndate);
  toTime = makeTime(tm);
}


void loop() {
  checkButton();
  timeClient.update();  
  TimeChangeRule nlDST = {"DST", Last, Sun, Mar, 1, +120};  //UTC + 2 hours
  TimeChangeRule nlSTD = {"STD", Last, Sun, Oct, 1, +60};   //UTC + 1 hour
  Timezone MyTZ(nlDST, nlSTD);
  time_t local_time = MyTZ.toLocal(timeClient.getEpochTime());

  toDateSeconds = (toTime - local_time);
    
  if ((checkDST(toTime)==0) and (checkDST(local_time)==1)) toDateSeconds += 3600;
  if ((checkDST(toTime)==1) and (checkDST(local_time)==0)) toDateSeconds -= 3600;
  if (toDateSeconds < 0) toDateSeconds = 0;
  toDateMinutes = floor(toDateSeconds / 60);
  toDateHours = floor(toDateMinutes / 60);
  toDateDays = floor(toDateHours / 24);

  countDownDays = toDateDays;
  countDownHours = toDateHours; 
  countDownMinutes = toDateMinutes - ((toDateHours -(countDownDays * 24)) * 60) - ((countDownDays *24) * 60); 
  countDownSeconds = toDateSeconds - (countDownMinutes * 60) - (((toDateHours -(countDownDays * 24)) * 60) *60) - (((countDownDays * 24) * 60) * 60); 
  
  if (countDownHours > 999){
  countDownHours = 999;
  countDownMinutes = 99;
  countDownSeconds = 99;
  }
  
  Serial.print(countDownHours);
  Serial.print(":");
  Serial.print(countDownMinutes);
  Serial.print(":");
  Serial.println(countDownSeconds);
  delay(1000);

/*  digitalWrite(strobePin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[countDownSeconds % 10]);  
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownSeconds /10) % 10]);  
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[10]);
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[countDownMinutes % 10]);  
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownMinutes /10) % 10]);  
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownMinutes /100) % 10]);  
  digitalWrite(strobePin, HIGH);
  */
}

void tick()
{
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void configModeCallback (WiFiManager *myWiFiManager) {
  
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

int checkDST(time_t dt){
int DST = 0;
if (month(dt)<3 || month(dt)>10) DST=0; 
if (month(dt)>3 && month(dt)<10) DST=1; 
if (month(dt)==3 && day(dt)<25) DST=0; 
if (month(dt)==10 && day(dt)<25) DST=1; 
if ((month(dt)==3 && hour(dt) + 24 * day(dt))>=(1 + 24*(31 - (5 * year(dt) /4 + 4) % 7)) or (month(dt)==10 && (hour(dt) + 24 * day(dt))<(1 + 24*(31 - (5 * year(dt) /4 + 1) % 7)))) DST=1;
return DST;
}

void createElements(const char *str)
{
  sscanf(str, "%d-%d-%d %d:%d:%d", &calcYear, &calcMonth, &calcDay, &calcHour, &calcMinute, &calcSecond);
  tm.Year = CalendarYrToTm(calcYear);
  tm.Month = calcMonth;
  tm.Day = calcDay;
  tm.Hour = calcHour;
  tm.Minute = calcMinute;
  tm.Second = calcSecond;
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
  if (!wm.startConfigPortal("TalkTimer")) {
    Serial.println("failed to connect and hit timeout");
    ESP.restart();
    delay(1000);
  }
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
  ESP.restart();
}  
