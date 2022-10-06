#include <ArduinoJson.h>
#include <LittleFS.h>
#include <FS.h> 
#include <WiFiManager.h>
#include <Ticker.h>

char countdowndate[19];
char fscountdowndate[19];

Ticker ticker;

void setup() {
  randomSeed(micros());
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
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

  WiFiManagerParameter custom_countdown_date("date", "countdown to: yyyy:MM:dd:HH:mm", countdowndate, 19);
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
}


void loop() {

        Serial.println(countdowndate);
        delay(2000);
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
