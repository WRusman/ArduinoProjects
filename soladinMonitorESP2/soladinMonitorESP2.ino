#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <SoftwareSerial.h>
#include <Soladin.h>

SoftwareSerial solCom(12, 13);  // D6 D7  serial to connect to soladin  
Soladin sol ;                 

WiFiClient espClient;
PubSubClient client(espClient);
Ticker ticker; 
WiFiManager wifiManager;

boolean solConnect = false;
unsigned long last;
byte retries = 0;
boolean successQ = false;

const char* mqtt_server = "mqtt.knuterboas.nl";
const char* mqtt_user = "monitor";
const char* mqtt_pass = "6d3lX0u1WOoyAu4E";
const char* PVVoltTopic = "Voermanstraat/solar/PVVolt";
const char* PVAmpTopic = "Voermanstraat/solar/PVAmp";
const char* GridFreqTopic = "Voermanstraat/solar/GridFreq";
const char* GridVoltTopic = "Voermanstraat/solar/GridVolt";
const char* GridPowerTopic = "Voermanstraat/solar/GridPower";
const char* DevTempTopic = "Voermanstraat/solar/DeviceTemp";
const char* TotalPowerTopic = "Voermanstraat/solar/TotalPower";

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect("Soladin")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset(); 
    delay(1000);
  }
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);

  solCom.begin(9600); 
  sol.begin(&solCom);

  last = millis();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  retries = 0;
  while ((! solConnect) && (retries <  6)){
   Serial.println("trying connection to soladin");
   solConnect = sol.query(PRB);    
   delay(1000*retries);
    retries++;
   Serial.print("tries = ");
   Serial.println(retries);
  } 
    
    if (((long)(millis()-last) > 60000) && (WiFi.status() == WL_CONNECTED) && solConnect) {
      successQ = false;
      retries = 0;
        while ((! successQ) && (retries < 6)) {      
          Serial.println("query soladin");
          successQ = sol.query(DVS);
          if (! successQ) {
            delay(1000*retries);
            retries++;
          }            
        }
      solConnect = successQ;   
      if (solConnect) {
        Serial.print("PV Voltage :");
        Serial.println(String(float(sol.PVvolt)/10).c_str());
        Serial.print("V \n PV Current :");
        Serial.println(String(float(sol.PVamp)/100).c_str());
        Serial.print("A \n Grid Freuqncy :");
        Serial.println(String(float(sol.Gridfreq)/100).c_str());
        Serial.print("Hz \n Grid Voltage :");
        Serial.println(String(sol.Gridvolt).c_str());
        Serial.print("V \n Current Grid Power  :");
        Serial.println(String(sol.Gridpower).c_str());
        Serial.print("W \n Device Temperature :");
        Serial.println(String(sol.DeviceTemp).c_str());
        Serial.print("C \n Total Power :");
        Serial.println(String(float(sol.Totalpower)/100).c_str());
        Serial.print("kWh");
        
        client.publish(PVVoltTopic, String(float(sol.PVvolt)/10).c_str());
        client.publish(PVAmpTopic, String(float(sol.PVamp)/100).c_str());
        client.publish(GridFreqTopic, String(float(sol.Gridfreq)/100).c_str());
        client.publish(GridVoltTopic, String(sol.Gridvolt).c_str());
        client.publish(GridPowerTopic, String(sol.Gridpower).c_str());
        client.publish(DevTempTopic, String(sol.DeviceTemp).c_str());
        client.publish(TotalPowerTopic, String(float(sol.Totalpower)/100).c_str());
        last = millis();        
      }
    }
  
  delay(1000+(retries*10000));
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
    if (client.connect(clientId.c_str(),mqtt_user, mqtt_pass)) {
      Serial.println("connected");  
      client.subscribe(PVVoltTopic);  
      client.subscribe(PVAmpTopic);  
      client.subscribe(GridFreqTopic);  
      client.subscribe(GridVoltTopic);  
      client.subscribe(GridPowerTopic);  
      client.subscribe(DevTempTopic);  
      client.subscribe(TotalPowerTopic);  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
