#include <ESP8266WiFi.h>          
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          
#include <Ticker.h>
#include "DHTesp.h"
#include <Wire.h>
#include <PubSubClient.h>


Ticker ticker;

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;

#define mqtt_server       "mqtt.knuterboas.nl"
#define mqtt_port         "1883"
#define mqtt_user         "monitor"
#define mqtt_pass         "6d3lX0u1WOoyAu4E"
#define mqtt_temptopic    "smitsborg/temperature/sensor1"
#define mqtt_humtopic     "smitsborg/humidity/sensor1"

long lastMsg = 0;
char msg[50];
int value = 0;
float temperature, humidity;
const int dhtpin = 2; //GPIO2 = D4

void setup() {
  randomSeed(micros());
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  dht.setup(dhtpin, DHTesp::DHT22);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ticker.attach(0.6, tick);
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect("espThermometer")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);
}

void loop() {
  if (!client.connected()) {
    connect_mqtt();
  }
  client.loop();
 
  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;
    temperature = dht.getTemperature();
    humidity = dht.getHumidity();
    Serial.print("Temperature : ");
    Serial.println(String(temperature).c_str());
    Serial.print("Humidity : ");
    Serial.println(String(humidity).c_str());

    client.publish(mqtt_temptopic, String(temperature).c_str());
    client.publish(mqtt_humtopic, String(humidity).c_str());
  }
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
  
void connect_mqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(),mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(mqtt_temptopic);
      client.subscribe(mqtt_humtopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
