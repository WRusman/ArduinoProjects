#include <ArduinoJson.h>
#include <LittleFS.h>
#include <FS.h> 
#include <WiFiManager.h>
#include <Ticker.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>          
#include <DNSServer.h>
#include <WiFiUdp.h>

tmElements_t tm;

static const char ntpServerName[] = "nl.pool.ntp.org";
const int timeZone = 2;             // Central European Time
int strobePin = 3;
int dataPin = 4;
int clockPin = 5;
byte segChar[] = { 246, 48, 110, 122, 184, 218, 222, 112, 254, 250, 127, 0};
int hour1,hour2,minute1,minute2,second1,second2,nowhours,nowminutes,nowseconds;
int calcYear, calcMonth, calcDay, calcHour, calcMinute, calcSecond, tddays, tdhours, tdminutes, tdseconds, cddays, cdhours, cdminutes,cdseconds,DSTactive ;
bool colon = true;
const int NTP_PACKET_SIZE = 48;     // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
unsigned int localPort = 8888;      // local port to listen for UDP packets
char countdowndate[16];
char fscountdowndate[16];

Ticker ticker;
WiFiUDP Udp;
time_t getNtpTime();


void setup() {
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  colon=true;
  
  randomSeed(micros());
  Serial.begin(115200);
  ticker.attach(0.6, tick);

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

  WiFiManagerParameter custom_countdown_date("date", "countdown to: yyyy-MM-dd HH:mm", countdowndate, 16);
  WiFiManager wm;
  wm.addParameter(&custom_countdown_date);
  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect("countdownclock")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  strcpy(countdowndate, custom_countdown_date.getValue());      
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
  if (fscountdowndate != countdowndate ) {
    if (!saveConfig()) {
      Serial.println("Failed to save config");
      } else {
      Serial.println("Config saved");
      }
  }
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  
}


void loop() {
  DSTactive=checkDST();
  time_t local = now();
  createElements(countdowndate);
  unsigned long toTime = makeTime(tm);
  
  tdseconds = (toTime - local);
  tdminutes = (tdseconds / 60);
  tdhours = (tdminutes / 60);
  tddays = (tdhours / 24);
  
  cddays = floor(tddays);
  cdhours = floor(tdhours -(cddays * 24)); 
  cdminutes = floor(tdminutes - (cdhours * 60) - ((cddays *24) * 60)); 
  cdseconds = floor(tdseconds - (cdminutes * 60) - ((cdhours * 60) *60) - (((cddays * 24) * 60) * 60)); 

  Serial.print("DST active: ");
  Serial.println(checkDST());
  Serial.print("now: ");
  Serial.print(year());
  Serial.print("-");
  Serial.print(month());
  Serial.print("-");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(hour());
  Serial.print(":");
  Serial.println(minute());
  Serial.print("to:  ");
  Serial.println(countdowndate);
  Serial.print("countdown :");      
  Serial.print(cddays);
  Serial.print(":");
  Serial.print(cdhours);
  Serial.print(":");
  Serial.print(cdminutes);
  Serial.print(":");
  Serial.println(cdseconds);
  delay(5000);
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

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
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

time_t getNtpTime()
{
  IPAddress ntpServerIP; 
  while (Udp.parsePacket() > 0) ; 
  Serial.println("Transmit NTP Request");
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return (secsSince1900 - 2208988800UL + ((timeZone + DSTactive ) * SECS_PER_HOUR));
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

void sendNTPpacket(IPAddress &address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123); 
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

int checkDST(){
time_t dt = now();
int DST = 0;
if (month(dt)<3 || month(dt)>10) DST=0; 
if (month(dt)>3 && month(dt)<10) DST=1; 
if (month(dt)==3 && day(dt)<25) DST=0; 
if (month(dt)==10 && day(dt)<25) DST=1; 
if ((month(dt)==3 && hour(dt) + 24 * day(dt))>=(1 + 24*(31 - (5 * year(dt) /4 + 4) % 7)) or (month(dt)==10 && (hour(dt) + 24 * day(dt))<(1 + 24*(31 - (5 * year(dt) /4 + 1) % 7)))) DST=1;
return DST;
}

int checkdestDST(){
unsigned long toTime = makeTime(tm);
int destDST = 0;
if (month(toTime)<3 || month(toTime)>10) destDST=0; 
if (month(toTime)>3 && month(toTime)<10) destDST=1; 
if (month(toTime)==3 && day(toTime)<25) destDST=0; 
if (month(toTime)==10 && day(toTime)<25) destDST=1; 
if ((month(toTime)==3 && hour(toTime) + 24 * day(toTime))>=(1 + 24*(31 - (5 * year(toTime) /4 + 4) % 7)) or (month(toTime)==10 && (hour(toTime) + 24 * day(toTime))<(1 + 24*(31 - (5 * year(toTime) /4 + 1) % 7)))) destDST=1;
return destDST;
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
