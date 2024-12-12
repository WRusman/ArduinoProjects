#include <Timezone.h>             //https://github.com/JChristensen/Timezone
#include <Ticker.h>               //https://github.com/sstaub/Ticker
#include <WiFi.h>                 //https://github.com/espressif/arduino-esp32/ 
#include <NTPClient.h>            //https://github.com/arduino-libraries/NTPClient
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ESPAsyncWebServer.h>    //https://github.com/me-no-dev/ESPAsyncWebServer
#include "defines.h"              //declaration of variables etc
#include <ArduinoJson.h>          //https://github.com/arduino-libraries/Arduino_JSON/
#include <LittleFS.h>             //https://github.com/espressif/arduino-esp32/
#include <FS.h>                   //https://github.com/espressif/arduino-esp32/
#include "charset.h"              //character set for the Matrix display

void TaskMatrixDisplay(void *pvParameters);
void TaskBIGDisplay(void *pvParameters);
void TaskMakeTime(void *pvParameters);

int calcYear,calcMonth,calcDay,calcHour,calcMinute,calcSecond,toDateDays,toDateHours,toDateMinutes,toDateSeconds,countDownDays,countDownHours,countDownMinutes,countDownSeconds,diffUTC ;
const byte segChar[] = { 246, 48, 110, 122, 184, 218, 222, 112, 254, 250, 127, 0};
const byte rowPins[numRow] = {row1Pin,row2Pin,row3Pin,row4Pin,row5Pin,row6Pin,row7Pin};

char countDownDate[16], fscountDownDate[16];
unsigned long toTime;
volatile bool filesystemOK = false;

TimeChangeRule nlDST = {"DST", Last, Sun, Mar, 1, +120};  //UTC + 2 hours
TimeChangeRule nlSTD = {"STD", Last, Sun, Oct, 1, +60};   //UTC + 1 hour
Timezone MyTZ(nlDST, nlSTD);
time_t local_time;

uint8_t displayBuffer[numReg][numRow], textBuffer[numChar][numRow];
String message="";
String clockMode;
String targetTime="0000-00-00 00:00"; 

Ticker ticker;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
AsyncWebServer server(80);
tmElements_t tm;

#include "functions.h"            //Subfunctions
#include "index.h"                //Main Webpage
#include "settings.h"             //Webpage for settings


void setup() {
  pinMode(matrixOePin, OUTPUT);
  pinMode(matrixClkPin, OUTPUT);digitalWrite (matrixClkPin, LOW);
  pinMode(matrixDataPin, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(row1Pin, OUTPUT);digitalWrite (row1Pin, HIGH);
  pinMode(row2Pin, OUTPUT);digitalWrite (row2Pin, HIGH);
  pinMode(row3Pin, OUTPUT);digitalWrite (row3Pin, HIGH);
  pinMode(row4Pin, OUTPUT);digitalWrite (row4Pin, HIGH);
  pinMode(row5Pin, OUTPUT);digitalWrite (row5Pin, HIGH);
  pinMode(row6Pin, OUTPUT);digitalWrite (row6Pin, HIGH);
  pinMode(row7Pin, OUTPUT);digitalWrite (row7Pin, HIGH);
  Serial.begin(115200);

  xTaskCreatePinnedToCore(TaskMatrixDisplay, "Draw Matrix display", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskBIGDisplay, "Draw Big display", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskMakeTime, "calculate times", 2048, NULL, 1, NULL, 0);

  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);

  ticker.attach(0.6, tick);
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect("CountdownClock")) {
    Serial.println("failed to connect and hit timeout");
    ESP.restart();
    delay(1000);
  }
  
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, HIGH);
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
    String timeRemaining;
    String localTime = String(year(local_time)/1000 %10)+String(year(local_time)/100 %10)+String(year(local_time)/10 %10)+String(year(local_time)%10)+"-"+String(month(local_time)/10 %10)+String(month(local_time)%10)+"-"+String(day(local_time)/10 %10)+String(day(local_time) %10)+" "+String(hour(local_time)/10 %10)+String(hour(local_time) %10)+":"+String (minute(local_time)/10 %10)+String (minute(local_time) %10)+":"+String (second(local_time)/10 %10)+String (second(local_time) %10);
    timeRemaining = String((countDownDays)/100 %10)+String((countDownDays)/10 %10)+String((countDownDays) %10)+":"+String((countDownHours)/10 %10)+String((countDownHours) %10)+":"+String((countDownMinutes)/10 %10)+String((countDownMinutes) %10)+":"+String((countDownSeconds/10) %10) +String((countDownSeconds) %10);
    request->send(200, "application/json","{\"localtime\":\""+localTime+"\",\"endtime\":\""+targetTime+"\",\"timeremaining\":\""+timeRemaining+"\",\"message\":\""+message+"\"}");
  });

  server.on("/setTargetTime", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.print("http get /setTargetTime");    
    if(request->hasArg("targetTime"))
    {
      targetTime = request->arg("targetTime");
      targetTime.replace("T", " ");
      message = request->arg("message");
      Serial.println("?targetTime="+targetTime+"&message="+message);
      saveConfig();
      message2Buffer();
    }
    request->send_P(200, "text/html", MAIN_page);     
  });

  server.on("/saveSettings", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("http get /saveSettings");
     clockMode = request->arg("clockMode");
     Serial.println("clock mode = "+ clockMode);
     saveConfig();
     request->send(200, "text/html", "Settings Saved");
  });


  server.onNotFound(notFound);
  server.begin();

  initFS();
  loadConfig();

  timeClient.begin();
  createElements(countDownDate);
  toTime = makeTime(tm);
}


void loop() {
    timeClient.update();   
    delay(100);   
} 

void TaskMatrixDisplay(void *pvParameters) { 
  (void)pvParameters;

  for (;;) {
    showBuffer();
//    delay(1);
  }
}

void TaskBIGDisplay(void *pvParameters) { 
  (void)pvParameters;

  for (;;) {
    sleep(1000);
  }
}

void TaskMakeTime(void *pvParameters) { 
  (void)pvParameters;

  for (;;) {
    local_time = MyTZ.toLocal(timeClient.getEpochTime());

    //struct tm *ptm = gmtime ((time_t *)&epochTime);
    //strcpy (countDownDate,(String(ptm->tm_year+1900) + "-" + String(ptm->tm_mon+1) + "-" + String(ptm->tm_mday) + " " + targetTime).c_str());
    //createElements(countDownDate);
    createElements((targetTime).c_str());
    
    toTime = makeTime(tm);

    
    toDateSeconds = (toTime - local_time);
  
    if ((checkDST(toTime)==0) and (checkDST(local_time)==1)) toDateSeconds += 3600;
    if ((checkDST(toTime)==1) and (checkDST(local_time)==0)) toDateSeconds -= 3600;
    if (toDateSeconds < 0) toDateSeconds = 0;

    toDateMinutes = floor(toDateSeconds / 60);
    toDateHours = floor(toDateMinutes / 60);
    toDateDays = floor(toDateHours / 24);
  
/*    countDownDays = toDateDays;
    countDownHours = toDateHours -(countDownDays * 24); 
    countDownMinutes = toDateMinutes - (countDownHours * 60) - ((countDownDays *24) * 60); 
    countDownSeconds = toDateSeconds - (toDateMinutes *60) - (countDownHours * 60) - ((countDownDays *24) * 60); 
  */
    countDownDays = toDateDays;
    countDownHours = toDateHours -(countDownDays * 24); 
    countDownMinutes = toDateMinutes - ((toDateHours -(countDownDays * 24)) * 60) - ((countDownDays *24) * 60); 
    countDownSeconds = toDateSeconds - (countDownMinutes * 60) - (((toDateHours -(countDownDays * 24)) * 60) *60) - (((countDownDays * 24) * 60) * 60);   
    delay(100);    
    }
}
