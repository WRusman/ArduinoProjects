#include <ESP8266WiFi.h>          
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          
#include <Ticker.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"
#include <PubSubClient.h>
#include "OLED.h"

Ticker ticker;
OLED display(4,5);
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_AM2320 am2320 = Adafruit_AM2320();

#define mqtt_server       "mqtt.knuterboas.nl"
#define mqtt_port         "1883"
#define mqtt_user         "monitor"
#define mqtt_pass         "<password>"
#define mqtt_temptopic1    "voermanstraat/buiten/temperature"
#define mqtt_humtopic1     "voermanstraat/buiten/humidity"
#define mqtt_temptopic2    "voermanstraat/binnen/temperature"
#define mqtt_humtopic2     "voermanstraat/binnen/humidity"


long lastMsg = 0;
char msg[50];
int value = 0;
float temperature, humidity;
char message[10];

void setup() {
  randomSeed(micros());
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  am2320.begin();
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
   
  Serial.println("connected! )");
  ticker.detach();
  digitalWrite(BUILTIN_LED, HIGH); //led off

  display.begin();
  display.clear();
}

void loop() {
  if (!client.connected()) {
    display.clear();
    display.print("Niet verbonden",0,1);
    connect_mqtt();
    display.print("Temp.     Hum.", 0,1);
    display.print("     Inside   ", 2,1);
    display.print("|",3 ,8);
    display.print("    Outside   ",5,1);
    display.print("|",6 ,8);
  }

  client.loop();
 
  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;
    temperature = am2320.readTemperature();
    humidity = am2320.readHumidity();
    Serial.print("Temperature : ");
    Serial.println(String(temperature).c_str());
    Serial.print("Humidity : ");
    Serial.println(String(humidity).c_str());

    client.publish(mqtt_temptopic2, String(temperature).c_str());
    client.publish(mqtt_humtopic2, String(humidity).c_str());
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
  Serial.print("mqtt message in topic [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  memcpy(message, payload,length);
  if (strcmp(topic,mqtt_temptopic2)==0)
  {
    display.print(message, 3, 1);
    Serial.println(message);  
  }

  if (strcmp(topic,mqtt_temptopic1)==0)
  {
    display.print(message, 6, 1);
    Serial.println(message);  
  }

  if (strcmp(topic,mqtt_humtopic2)==0)
  {
    display.print(message, 3, 10);
    Serial.println(message);  
  }

  if (strcmp(topic,mqtt_humtopic1)==0)
  {
    display.print(message, 6, 10);
    Serial.println(message);  
  }
}
  
void connect_mqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(),mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(mqtt_temptopic1);
      client.subscribe(mqtt_temptopic2);
      client.subscribe(mqtt_humtopic1);
      client.subscribe(mqtt_humtopic2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
