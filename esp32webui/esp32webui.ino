#include <WiFi.h>          
#include <WiFiManager.h>
#include <Ticker.h>
#include <ESPAsyncWebServer.h>
#include "index.h"  //Web page header file

String text="00:00", globalState="Off" ;

Ticker ticker;

AsyncWebServer server(80);

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);

  ticker.attach(0.6, tick);
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect("webUI")) {
    Serial.println("failed to connect and hit timeout");
    ESP.restart();
    delay(1000);
  }
  
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("http get /");
    request->send_P(200, "text/html", MAIN_page);
  });

  server.on("/getState", HTTP_GET, [](AsyncWebServerRequest *request){
//    Serial.println("http get /getState");
    request->send(200, "text/plain", globalState);
  });

  server.on("/setState", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.print("http get /setState");
    if(request->hasArg("state"))
     globalState = request->arg("state");
     Serial.println("?newState="+globalState);
    request->send(200, "text/plain", "ok");
  });

  server.on("/getText", HTTP_GET, [](AsyncWebServerRequest *request){
//    Serial.println("http get /getText");
    request->send(200, "text/plain", text);
  });

  server.on("/setText", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.print("http get /setText");    
    if(request->hasArg("text"))
    {
      text = request->arg("text");
      Serial.println("?text="+text);
    }
    request->send_P(200, "text/html", MAIN_page);
  });

  server.onNotFound(notFound);
  server.begin();
}


void loop() {
}

void tick()
{
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


void configModeCallback (WiFiManager *myWiFiManager) {
  
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
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
