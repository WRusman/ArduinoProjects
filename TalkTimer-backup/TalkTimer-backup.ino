#include <ArduinoJson.h>
#include <LittleFS.h>
#include <FS.h> 
#include <WiFi.h>          
#include <Timezone.h>             //https://github.com/JChristensen/Timezone
#include <NTPClient.h>            //https://github.com/arduino-libraries/NTPClient
#include <WiFiManager.h>
#include <Ticker.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>
#include "index.h"  //Main Webpage
#include "settings.h"  //Webpage for settings

struct RGB {                       
   byte r;
   byte g;
   byte b;
};

RGB red = { 255 , 0 , 0 };
RGB yellow = { 255 , 255 , 0 };
RGB blue = { 63 , 90 , 255 };
RGB green = { 67 , 179 , 64 };
RGB black = { 0 , 0 , 0 };
RGB white = { 255 , 255 , 255 };

int setupPin = 23; // pin 23 on the devkit v1 for setup mode switch
int calcYear,calcMonth,calcDay,calcHour,calcMinute,calcSecond,toDateMinutes,toDateSeconds,countDownMinutes,countDownSeconds,diffUTC,buttonState,lastButtonState = LOW ;

char countDownDate[16];
String talkEnd="00:00", clockState="Pause", clockMode , clockColor;
time_t local_time;
unsigned long toTime, lastDebounceTime = 0, debounceDelay = 3;
volatile bool filesystemOK = false;

Ticker ticker;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
AsyncWebServer server(80);
tmElements_t tm;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(107, 22, NEO_GRB + NEO_KHZ800); // pin 22

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(setupPin, INPUT);
  
  strip.setBrightness(30);  
  strip.begin();
  setleds(0,106,black);    // Turn the comple strip black or "off";  
  strip.show();
  
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
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("http get /");
    request->send_P(200, "text/html", MAIN_page);
  });


  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("http get /settings");
    request->send_P(200, "text/html", SETTINGS_page);
  });

  server.on("/readData", HTTP_GET, [](AsyncWebServerRequest *request){
    String localTime = String(hour(local_time)/10 %10)+String(hour(local_time) %10)+":"+String (minute(local_time)/10 %10)+String (minute(local_time) %10)+":"+String (second(local_time)/10 %10)+String (second(local_time) %10);
    String timeRemaining = String(countDownMinutes)+":"+String((countDownSeconds/10) %10) +String((countDownSeconds) %10);
    request->send(200, "application/json","{\"localtime\":\""+localTime+"\",\"endtime\":\""+talkEnd+"\",\"timeremaining\":\""+timeRemaining+"\",\"clockstate\":\""+clockState+"\",\"clockcolor\":\""+clockColor+"\"}");
  });

  server.on("/setClockState", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.print("http get /setClockState");
    if(request->hasArg("state"))
     clockState = request->arg("state");
     Serial.println("?newState="+clockState);
     request->send(200, "text/plain", "ok");
  });

  server.on("/setTalkEnd", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.print("http get /setTalkEnd");    
    if(request->hasArg("talkEnd"))
    {
      talkEnd = request->arg("talkEnd");
      Serial.println("?talkEnd="+talkEnd);
    }
    request->send_P(200, "text/html", MAIN_page);     
  });

  server.on("/saveSettings", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("http get /saveSettings");
     String formTimeZone = request->arg("timeZone");
     diffUTC = formTimeZone.toInt();
     Serial.println("diffUTC = "+ String(diffUTC));
     clockMode = request->arg("clockMode");
     Serial.println("clock mode = "+ clockMode);
     saveConfig();
     request->send(200, "text/html", "Settings Saved");
  });


  server.onNotFound(notFound);
  server.begin();

  initFS();
  loadConfig();
  clockColor="white";

  Serial.println("mode : "+ clockMode);
  Serial.println("difference with UTC : " + String(diffUTC));  
}


void loop() {
  checkButton();
  timeClient.update();  

    time_t epochTime = timeClient.getEpochTime();
    TimeChangeRule local = {"LOCAL", Last, Sun, Mar, 1, diffUTC};     // UTC +/- set timezone difference
    Timezone MyTime(local);    
    time_t local_epoch_time = MyTime.toLocal(timeClient.getEpochTime());
    local_time = local_epoch_time;

    struct tm *ptm = gmtime ((time_t *)&epochTime);
    strcpy (countDownDate,(String(ptm->tm_year+1900) + "-" + String(ptm->tm_mon+1) + "-" + String(ptm->tm_mday) + " " + talkEnd).c_str());
    
   
    createElements(countDownDate);
    toTime = makeTime(tm);

    if (clockState == "On")
    {
      toDateSeconds = (toTime - local_epoch_time);
    }

    if (clockMode == "TZ")
    {
      if (toDateSeconds < 0) toDateSeconds = 0;
      toDateMinutes = floor(toDateSeconds / 60);
             
      countDownMinutes = toDateMinutes; 
      countDownSeconds = toDateSeconds - (countDownMinutes * 60); 
    }  
    else
    {
      toDateMinutes = floor(toDateSeconds / 60);
      
      countDownMinutes = toDateMinutes; 
      if (toDateMinutes < 0)
        countDownSeconds = - (toDateSeconds - (countDownMinutes * 60)); 
      else
        countDownSeconds = (toDateSeconds - (countDownMinutes * 60)); 
    }  

    if (countDownMinutes > 999){
    countDownMinutes = 999;
    countDownSeconds = 99;
    }

    if (clockState == "On")
    {
      Serial.println(String(countDownMinutes)+":"+String((countDownSeconds/10) %10) +String((countDownSeconds) %10));
      if (countDownMinutes < 0 )clockColor="red";
      else if ((countDownMinutes < 5 ) and (countDownMinutes > 0 ))clockColor="orange";      
      else if ((countDownMinutes < 10 ) and (countDownMinutes > 5 ))clockColor="yellow";
      else clockColor="white";
    }
    delay(1000);
}

void tick()
{
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void initFS() {
  if (!LittleFS.begin(false /* false: Do not format if mount failed */)) {
    Serial.println("Failed to mount LittleFS");
    if (!LittleFS.begin(true /* true: format */)) {
      Serial.println("Failed to format LittleFS");
    } else {
      Serial.println("LittleFS formatted successfully");
      filesystemOK = true;
    }
  } else { // Initial mount success
    filesystemOK = true;
  }
}

void setleds(int startled, int stopled, RGB led_color) {
  for (int lednum = startled; lednum <= stopled; lednum++){
       strip.setPixelColor(lednum, led_color.r, led_color.g, led_color.b);
  }
}


bool loadConfig() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    clockMode="AO"; // default mode is "To Zero"
    diffUTC=0; // default timeZone is UTC 
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
  String fsDiffUTC = doc["timeZone"];
  diffUTC = fsDiffUTC.toInt();
  String fsClockMode = doc["clockMode"];
  clockMode=fsClockMode.c_str();
  return true;
}

bool saveConfig() {
  StaticJsonDocument<200> doc;
  doc["timeZone"] = diffUTC;
  doc["clockMode"] = clockMode;
  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  serializeJson(doc, configFile);
  return true;
}


void configModeCallback (WiFiManager *myWiFiManager) {
  
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
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
