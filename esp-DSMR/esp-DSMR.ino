#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include "dsmr.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(D5, -1, false );  //rx, tx, invert
WiFiClient espClient;
PubSubClient client(espClient);
Ticker ticker; 
WiFiManager wifiManager;
P1Reader reader(&mySerial,0);

const char* mqtt_server = "mqtt.knuterboas.nl";
const char* mqtt_user = "monitor";
const char* mqtt_pass = "6d3lX0u1WOoyAu4E";
const char* DSMRidTopic = "Voermanstraat/DSMR/deviceID";
const char* DSMRdelivered1Topic = "Voermanstraat/DSMR/eDeliveredl1";
const char* DSMRdelivered2Topic = "Voermanstraat/DSMR/eDeliveredl2";
const char* DSMRreturned1Topic = "Voermanstraat/DSMR/eReturnedl1";
const char* DSMRreturned2Topic = "Voermanstraat/DSMR/eReturnedl2";
const char* DSMRpDeliveredTopic = "Voermanstraat/DSMR/pDelivered";
const char* DSMRpReturnedTopic = "Voermanstraat/DSMR/pReturned";
const char* DSMRcurrentl1Topic = "Voermanstraat/DSMR/currentl1";
const char* DSMRcurrentl2Topic = "Voermanstraat/DSMR/currentl2";
const char* DSMRpDeliveredl1Topic = "Voermanstraat/DSMR/pDeliveredl1";
const char* DSMRpDeliveredl2Topic = "Voermanstraat/DSMR/pDeliveredl2";
const char* DSMRpReturnedl1Topic = "Voermanstraat/DSMR/pReturnedl1";
const char* DSMRpReturnedl2Topic = "Voermanstraat/DSMR/pReturnedl2";
const char* DSMRgasDeviceTopic = "Voermanstraat/DSMR/gasDevice";
const char* DSMRgasDeliveredTopic = "Voermanstraat/DSMR/gasDelivered";


using MyData = ParsedData<
  /* String */ identification,
  /* FixedValue */ energy_delivered_tariff1,
  /* FixedValue */ energy_delivered_tariff2,
  /* FixedValue */ energy_returned_tariff1,
  /* FixedValue */ energy_returned_tariff2,
  /* FixedValue */ power_delivered,
  /* FixedValue */ power_returned,
  /* FixedValue */ current_l1,
  /* FixedValue */ current_l2,
  /* FixedValue */ power_delivered_l1,
  /* FixedValue */ power_delivered_l2,
  /* FixedValue */ power_returned_l1,
  /* FixedValue */ power_returned_l2,  
  /* uint16_t */ gas_device_type,
  /* TimestampedFixedValue */ gas_delivered
>;

unsigned long last;
uint32_t telegramCount;
MyData   DSMRdata;

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect("DSMR")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset(); 
    delay(1000);
  }
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);

  mySerial.begin(115200, SWSERIAL_8N1); 
  reader.enable(true);
  last = millis();
  telegramCount   = 0;
  
  delay(200);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  reader.loop();

  // Every minute, fire off a one-off reading
  unsigned long now = millis();
  if (now - last > 60000) {
    reader.enable(true);
    last = now;
  }
  if (reader.available()) {
     telegramCount++;
     DSMRdata = {};
     String    DSMRerror;
        
        if (reader.parse(&DSMRdata, &DSMRerror))   // Parse succesful, print result
        {
          if (telegramCount > 1563000000) 
          {
            delay(1000);
            ESP.reset();
            delay(1000);
          }

          Serial.print("Device ID :");
          Serial.println(String(DSMRdata.identification).c_str());
          Serial.print("\n Delivered Tariff 1 :");
          Serial.println(String(DSMRdata.energy_delivered_tariff1).c_str());
          Serial.print("kWh \n Delivered Tariff 2 :");
          Serial.println(String(DSMRdata.energy_delivered_tariff2).c_str());
          Serial.print("kWh \n Returned Tariff 1 :");
          Serial.println(String(DSMRdata.energy_returned_tariff1).c_str());
          Serial.print("kWh \n Returned Tariff 2 :");
          Serial.println(String(DSMRdata.energy_returned_tariff2).c_str());
          Serial.print("kWh \n Delivered Power :");
          Serial.println(String(DSMRdata.power_delivered).c_str());
          Serial.print("kW \n Returned Power :");
          Serial.println(String(DSMRdata.power_returned).c_str());
          Serial.print("kW \n Current l1 :");
          Serial.println(String(DSMRdata.current_l1).c_str());
          Serial.print("A \n Current l2 :");
          Serial.println(String(DSMRdata.current_l2).c_str());
          Serial.print("A \n Delivered Power l1 :");
          Serial.println(String(DSMRdata.power_delivered_l1).c_str());
          Serial.print("kW \n Delivered Power l2 :");
          Serial.println(String(DSMRdata.power_delivered_l2).c_str());
          Serial.print("kW \n Returned Power l1 :");
          Serial.println(String(DSMRdata.power_returned_l1).c_str());
          Serial.print("kW \n Returned Power l2 :");
          Serial.println(String(DSMRdata.power_returned_l2).c_str());
          Serial.print("kW \n Gas Device :");
          Serial.println(String(DSMRdata.gas_device_type).c_str());
          Serial.print("\n Delivered Gas :");          
          Serial.println(String(DSMRdata.gas_delivered).c_str());
          Serial.println("m3");     
          
          client.publish(DSMRidTopic,String(DSMRdata.identification).c_str());
          client.publish(DSMRdelivered1Topic,String(DSMRdata.energy_delivered_tariff1).c_str());
          client.publish(DSMRdelivered2Topic,String(DSMRdata.energy_delivered_tariff2).c_str());
          client.publish(DSMRreturned1Topic,String(DSMRdata.energy_returned_tariff1).c_str());
          client.publish(DSMRreturned2Topic,String(DSMRdata.energy_returned_tariff2).c_str());
          client.publish(DSMRpDeliveredTopic,String(DSMRdata.power_delivered).c_str());
          client.publish(DSMRpReturnedTopic,String(DSMRdata.power_returned).c_str());
          client.publish(DSMRcurrentl1Topic,String(DSMRdata.current_l1).c_str());
          client.publish(DSMRcurrentl2Topic,String(DSMRdata.current_l2).c_str());
          client.publish(DSMRpDeliveredl1Topic,String(DSMRdata.power_delivered_l1).c_str());
          client.publish(DSMRpDeliveredl2Topic,String(DSMRdata.power_delivered_l2).c_str());
          client.publish(DSMRpReturnedl1Topic,String(DSMRdata.power_returned_l1).c_str());
          client.publish(DSMRpReturnedl2Topic,String(DSMRdata.power_returned_l2).c_str());
          client.publish(DSMRgasDeviceTopic,String(DSMRdata.gas_device_type).c_str());
          client.publish(DSMRgasDeliveredTopic,String(DSMRdata.gas_delivered).c_str());
    }
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
      client.subscribe(DSMRidTopic);  
      client.subscribe(DSMRdelivered1Topic);  
      client.subscribe(DSMRdelivered2Topic);  
      client.subscribe(DSMRreturned1Topic);  
      client.subscribe(DSMRreturned2Topic);
      client.subscribe(DSMRpDeliveredTopic);
      client.subscribe(DSMRpReturnedTopic);
      client.subscribe(DSMRcurrentl1Topic);
      client.subscribe(DSMRcurrentl2Topic);
      client.subscribe(DSMRpDeliveredl1Topic);
      client.subscribe(DSMRpDeliveredl2Topic);
      client.subscribe(DSMRpReturnedl1Topic);
      client.subscribe(DSMRpReturnedl2Topic);
      client.subscribe(DSMRgasDeviceTopic);
      client.subscribe(DSMRgasDeliveredTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
