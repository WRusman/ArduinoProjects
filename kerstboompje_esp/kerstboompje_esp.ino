#include <ESP8266WiFi.h>     
#include <ESP8266WebServer.h>
#include <EmbAJAX.h>
#include <WiFiManager.h>          
#include <Ticker.h>
#include <Adafruit_NeoPixel.h>

Ticker ticker;
WiFiClient espClient;

#define PIN 4
#define NUM_LEDS 144
#define NUM_SPARKLE 26
#define BUFLEN 30
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int leds[NUM_SPARKLE] = {0,4,10,16,22,27,33,37,44,50,58,63,70,74,79,85,90,97,102,107,114,120,126,132,138,143} ;
EmbAJAXOutputDriverWebServerClass server(80);
EmbAJAXOutputDriver driver(&server);
EmbAJAXStatic nextCell("</td><td>&nbsp;</td><td><b>");
EmbAJAXStatic nextRow("</b></td></tr><tr><td>");
EmbAJAXColorPicker color("color", 0, 255, 255);
EmbAJAXMutableSpan color_d("color_d");
char color_d_buf[BUFLEN];

MAKE_EmbAJAXPage(page, "EmbAJAX example - Inputs", "",
    new EmbAJAXStatic("<table cellpadding=\"10\"><tr><td>"),
    new EmbAJAXStatic("kerstballen kleur:"),
    &nextCell,
    &color,
    &nextCell,
    //&color_d,
    &nextRow,
    new EmbAJAXStatic("Server status:"),
    &nextCell,
    new EmbAJAXConnectionIndicator(),
    new EmbAJAXStatic("</b></td></tr></table>")
)
void setup(){
  strip.begin();
  setAll(0,0x05,0);
  randomSeed(micros());
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect("kerstboompje")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
   
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, HIGH);
  driver.installPage(&page, "/", updateUI);
  server.begin();
}

void loop() {
    driver.loopHook();
 sparkle(leds[random(NUM_SPARKLE)]);
}

void sparkle (int sparkleLed) {
 for (int l=0; l<= NUM_SPARKLE; l++)
 {
  strip.setPixelColor(leds[l],0x10,0x00,0x00);
 }
 strip.show();

 for (int s=10; s<= 64; s = s + random(3))
  {
   strip.setPixelColor(sparkleLed,s*4,0,0);
   strip.show();
  }
   for (int s=125; s>= 10; s = s - random(3))
  {
   strip.setPixelColor(sparkleLed,s*2,0,0);
   strip.show();
  }
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
   strip.setPixelColor(i,red,green,blue);
  }
  strip.show();
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


void updateUI() {
    color_d.setValue(strncpy(color_d_buf, color.value(), BUFLEN)); 
}  