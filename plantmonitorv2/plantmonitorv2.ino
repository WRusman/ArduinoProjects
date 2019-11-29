#include "DHTesp.h"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <OneWire.h>

#define ONE_HOUR 3600000UL

const char* ssid     = "ttn";
const char* password = "belachelijkmakkelijk";
const char* mqtt_server = "192.168.1.138";
const char* ntpServerName = "nl.pool.ntp.org";
WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP UDP;
IPAddress timeServerIP;
DHTesp dht;

String temperatureString = "";
String humidityString = "";
String soilMoistString = "";
char msg[50];
const int statusLed = 12;
const int moistPin = A0;
const int dhtpin = 5; //GPIO5 = D1
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

const unsigned long intervalTemp = 60000;
unsigned long prevTemp = 0;
bool tmpRequested = false;
uint32_t timeUNIX = 0;

unsigned long now = millis();
unsigned long lastMeasure = 0;
boolean startTimer = false;
unsigned long currentMillis;
const unsigned long intervalNTP = ONE_HOUR;
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = millis();

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  
  reconnect();
  UDP.begin(123);
  dht.setup(dhtpin, DHTesp::DHT22);
  WiFi.hostByName(ntpServerName, timeServerIP);
  Serial.print("Time server IP:\t");
  Serial.println(timeServerIP);
  sendNTPpacket(timeServerIP);
  delay(500);
  client.setServer(mqtt_server, 1883);
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


  now = millis();
  if (now - lastMeasure > 30000) {
    lastMeasure = now;
    uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse) / 1000;

    temperatureString = String(dht.getTemperature());
    humidityString = String(dht.getHumidity());
    soilMoistString = String(map(analogRead(A0),700,400,0,100));
    
      Serial.print("Humidity: \t");
      Serial.print(humidityString);
      Serial.print("\nTemperature: \t");
      Serial.print(temperatureString);
      Serial.print("\nSoil Moisture: \t");
      Serial.println(soilMoistString);
      Serial.print("\n");
      
    client.publish("esp8266/temperature", temperatureString.c_str());
  //  Serial.println("Temperature published");
    
    client.publish("esp8266/humidity", humidityString.c_str());
  //  Serial.println("Humidity published");

    client.publish("esp8266/soilmoist", soilMoistString.c_str());
  //  Serial.println("Soil moisture published");
    }
  }


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void sendNTPpacket(IPAddress& address) {
  Serial.println("Sending NTP request");
  memset(packetBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode

  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(packetBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

unsigned long getTime() { // Check if the time server has responded, if so, get the UNIX time, otherwise, return 0
  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (packetBuffer[40] << 24) | (packetBuffer[41] << 16) | (packetBuffer[42] << 8) | packetBuffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
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
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");  
      client.publish("esp8266/temperature", "online");
      client.subscribe("esp8266/temperature");  
      client.publish("esp8266/humidity", "online");
      client.subscribe("esp8266/humidity");  
      client.publish("esp8266/soilmoist", "online");
      client.subscribe("esp8266/soilmoist");  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
