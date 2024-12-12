#include <ArduinoJson.h>
#include <LittleFS.h>
#include <FS.h> 
#include <ESP8266WiFi.h>          
#include <Timezone.h>             //https://github.com/JChristensen/Timezone
#include <NTPClient.h>            //https://github.com/arduino-libraries/NTPClient
#include <WiFiManager.h>
#include <Ticker.h>

int strobePin = 13;  // D7 on the wemos D1 mini
int dataPin = 4;     // D2 on the wemos D1 mini

int clockPin = 0;   // D3 on the wemos D1 mini
int setupPin = 15;  // D8 on the wemos D1 mini
int modePin = 12;   // D6 on the wemos D1 mini
// BUILTIN_LED = 2  // D4 on the wemos D1 mini, LOW for ON

int calcYear,calcMonth,calcDay,calcHour,calcMinute,calcSecond,toDateDays,toDateHours,toDateMinutes,toDateSeconds,countDownDays,countDownHours,countDownMinutes,countDownSeconds,buttonState,lastButtonState = LOW ;

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
tmElements_t tm;

void setup() {
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  
  pinMode(setupPin, INPUT);
  pinMode(modePin, INPUT);
  
  digitalWrite(strobePin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 236);  //p
  shiftOut(dataPin, clockPin, MSBFIRST, 182);  //u
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 142);  //t
  shiftOut(dataPin, clockPin, MSBFIRST, 12);   //r
  shiftOut(dataPin, clockPin, MSBFIRST, 0);  
  shiftOut(dataPin, clockPin, MSBFIRST, 126);  //a  
  shiftOut(dataPin, clockPin, MSBFIRST, 142);  //t      
  shiftOut(dataPin, clockPin, MSBFIRST, 218);  //S  
  digitalWrite(strobePin, HIGH);
 
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);

  Serial.println("Mounting FS...");
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
  if (!loadConfig()) {
    Serial.println("Failed to load config");
    } else {
    Serial.println("Config loaded");
    Serial.println(fscountdowndate);
  }
    ticker.attach(0.6, tick);
  WiFiManagerParameter custom_countdown_date("date", "countdown to: yyyy-MM-dd HH:mm", countdowndate, 16);
  WiFiManager wm;
  wm.addParameter(&custom_countdown_date);
  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect("countdownclock")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
  strcpy(countdowndate, custom_countdown_date.getValue());      
  if (fscountdowndate != countdowndate ) {
    if (!saveConfig()) {
      Serial.println("Failed to save config");
      } else {
      Serial.println("Config saved");
      }
  }
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

  if (digitalRead(modePin) == 1) {
    countDownDays = toDateDays;
    countDownHours = toDateHours -(countDownDays * 24); 
    countDownMinutes = toDateMinutes - (countDownHours * 60) - ((countDownDays *24) * 60); 
  
    Serial.print(countDownDays);
    Serial.print(":");
    Serial.print(countDownHours);
    Serial.print(":");
    Serial.println(countDownMinutes);
    delay(1000);
  
    digitalWrite(strobePin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[countDownMinutes % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownMinutes /10) % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[10]);
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[countDownHours % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownHours/10) % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[10]);
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[countDownDays % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownDays/10) % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownDays/100) % 10]);  
    digitalWrite(strobePin, HIGH);
  } 
  else {
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
  
    digitalWrite(strobePin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[countDownSeconds % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownSeconds /10) % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[10]);
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[countDownMinutes % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownMinutes /10) % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[10]);
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[countDownHours % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownHours/10) % 10]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[(countDownHours/100) % 10]);  
    digitalWrite(strobePin, HIGH);
  }
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
  shiftOut(dataPin, clockPin, MSBFIRST, 236);  //p
  shiftOut(dataPin, clockPin, MSBFIRST, 182);  //u
  shiftOut(dataPin, clockPin, MSBFIRST, 0);  
  shiftOut(dataPin, clockPin, MSBFIRST, 142);  //t     
  shiftOut(dataPin, clockPin, MSBFIRST, 238);  //e     
  shiftOut(dataPin, clockPin, MSBFIRST, 218);  //S  
  digitalWrite(strobePin, HIGH);
  
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

bool loadConfig() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* fscountdowndate = doc["countdowndate"];

  Serial.print("Loaded countdowndate: ");
  strcpy(countdowndate, fscountdowndate);
  Serial.println(countdowndate);
  return true;
}

bool saveConfig() {
  StaticJsonDocument<200> doc;
  doc["countdowndate"] = countdowndate;

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  serializeJson(doc, configFile);
  return true;
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
  WiFiManagerParameter custom_countdown_date("date", "countdown to: yyyy-MM-dd HH:mm", countdowndate, 16);
  WiFiManager wm;
  wm.addParameter(&custom_countdown_date);
  wm.setAPCallback(configModeCallback);
  if (!wm.startConfigPortal("countdownclock")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
  strcpy(countdowndate, custom_countdown_date.getValue());      
  if (!saveConfig()) {
    Serial.println("Failed to save config");
  } else {
    Serial.println("Config saved");
  }
  ESP.reset();
}  
