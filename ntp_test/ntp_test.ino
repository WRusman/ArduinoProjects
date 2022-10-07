#include <Timezone.h>             //https://github.com/JChristensen/Timezone
#include <NTPClient.h>            //https://github.com/arduino-libraries/NTPClient
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Ticker.h>

Ticker ticker;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

unsigned long ntp_epoch;

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);
  Serial.println("\nStarting...\n");
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect("ntptest")) {
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
  timeClient.update();  
  time_t local_time;
  TimeChangeRule *tcr;
  TimeChangeRule nlDST = {"DST", Last, Sun, Mar, 1, +120};  //UTC + 2 hours
  TimeChangeRule nlSTD = {"STD", Last, Sun, Oct, 1, +60};   //UTC + 1 hour
  Timezone MyTZ(nlDST, nlSTD);
  ntp_epoch = timeClient.getEpochTime();
  local_time = MyTZ.toLocal(ntp_epoch, &tcr); //corrected_ntp_time is NTP time with TZ + DST correction
  
  Serial.print("Current time :");
  Serial.print(year(local_time));
  Serial.print("-");
  Serial.print(month(local_time));
  Serial.print("-");
  Serial.print(day(local_time));
  Serial.print(" ");
  Serial.print(hour(local_time));
  Serial.print(":");
  Serial.print(minute(local_time));
  Serial.print(":");
  Serial.println(second(local_time));
  delay(10000);
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
