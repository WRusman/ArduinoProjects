#include "DHTesp.h"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <Ticker.h>

#define ONE_HOUR 3600000UL

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP UDP;
IPAddress timeServerIP;
DHTesp dht;
Ticker ticker; 
WiFiManager wifiManager;

#define MQTT_SERVER "mqtt.knuterboas.nl"
const char* mqtt_server = MQTT_SERVER;
const char* ntpServerName = "nl.pool.ntp.org";
const int statusLed = 12;
const int dhtpin = 5; //GPIO5 = D1
const int NTP_PACKET_SIZE = 48;
const unsigned long intervalTemp = 60000;
const unsigned long intervalNTP = ONE_HOUR;

char* temperatureTopic = "smitsborg/0164/temperature";
char* humidityTopic = "smitsborg/0164/humidity";
byte packetBuffer[NTP_PACKET_SIZE];
String temperatureString = "";
String humidityString = "";
uint32_t timeUNIX = 0;
unsigned long prevTemp = 0;
unsigned long now = millis();
unsigned long lastMeasure = 0;
unsigned long currentMillis;
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = millis();
boolean startTimer = false;
bool tmpRequested = false;

void setup() {
  Serial.begin(115200);
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

  delay(200);

  WiFi.hostByName(ntpServerName, timeServerIP);
  Serial.print("Time server IP:\t");
  Serial.println(timeServerIP);
  sendNTPpacket(timeServerIP);
  delay(500);
  UDP.begin(123);
  dht.setup(dhtpin, DHTesp::DHT22);

  client.setServer(mqtt_server, 1883);
  
  client.setCallback(callback);  
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  now = millis();
  if (now - prevNTP > intervalNTP) {
    prevNTP = now;
    sendNTPpacket(timeServerIP);
  }

  uint32_t time = getTime();
  if (time) {
    timeUNIX = time;
    Serial.print("NTP response:\t");
    Serial.println(timeUNIX);
    lastNTPResponse = millis();
  } else if ((millis() - lastNTPResponse) > 24UL * ONE_HOUR) {
    Serial.println("More than 24 hours since last NTP response. Rebooting.");
    Serial.flush();
    ESP.reset();
  }

  if (now - lastMeasure > 30000) {
    lastMeasure = now;
    uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse) / 1000;

    temperatureString = String(dht.getTemperature());
    humidityString = String(dht.getHumidity());
    Serial.print("Humidity: \t");
    Serial.print(humidityString);
    Serial.print("\nTemperature: \t");
    Serial.print(temperatureString);
    Serial.print("\n");
    client.publish(temperatureTopic, temperatureString.c_str());
    client.publish(humidityTopic, humidityString.c_str());
    }
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
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void sendNTPpacket(IPAddress& address) {
  Serial.println("Sending NTP request");
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  UDP.beginPacket(address, 123);
  UDP.write(packetBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

unsigned long getTime() {
  if (UDP.parsePacket() == 0) {
    return 0;
  }
  UDP.read(packetBuffer, NTP_PACKET_SIZE);
  uint32_t NTPTime = (packetBuffer[40] << 24) | (packetBuffer[41] << 16) | (packetBuffer[42] << 8) | packetBuffer[43];
  const uint32_t seventyYears = 2208988800UL;
  uint32_t UNIXTime = NTPTime - seventyYears;
  return UNIXTime;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.print("\n");  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(),"monitor","6d3lX0u1WOoyAu4E")) {
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
