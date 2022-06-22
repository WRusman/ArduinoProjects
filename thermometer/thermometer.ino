#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include "OLED.h"

WiFiClient espClient;
PubSubClient client(espClient);
Ticker ticker; 
WiFiManager wifiManager;
OLED display();

const char* mqtt_server = "192.168.1.136";
const char* ntpServerName = "nl.pool.ntp.org";
const int statusLed = 12;

char* temperatureTopic = "esp8266/temperature";
char* humidityTopic = "esp8266/humidity";
char* temperatureArray = "";
char* humidityArray = "";
String humidityString;
String temperatureString;
bool tmpRequested = false;

void setup() {
  Serial.begin(115200);
  display.begin();
  display.clear();  
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect("thermometer")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset(); 
    delay(1000);
  }
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");  
      client.subscribe(temperatureTopic);  
      client.subscribe(humidityTopic);  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
if (!client.connected()) {
  reconnect();
  }
  client.loop();
}

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

void callback(char* topic, byte* payload, unsigned int length) {
  byte* p = (byte*)malloc(length);
  char buff_p[length];
  for (int i = 0; i < length; i++)
  {
    buff_p[i] = (char)payload[i];
  }
  buff_p[length] = '\0';
  String msg_p = String(buff_p);
  String topicString = String(topic);
  free(p);
    
  if (topicString == "esp8266/temperature"){
      temperatureString = msg_p;
      temperatureString.toCharArray(temperatureArray,length);
      }      
  else if (topicString == "esp8266/humidity"){
      humidityString = msg_p;      
      humidityString.toCharArray(humidityArray,length);
      }
    display.clear();
    display.print("Temperature:",2, 4);
    display.print(temperatureArray,3, 6);  
    display.print("Humidity:",5, 4);
    display.print(humidityArray,6, 6);  

    Serial.println(topicString);
    Serial.println(msg_p);
    Serial.print("Temperature: ");
    Serial.println(temperatureString);
    Serial.print("Humidity: ");
    Serial.println(humidityString);


}
