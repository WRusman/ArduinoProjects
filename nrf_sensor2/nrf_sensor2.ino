#include <Sleep_n0m1.h>
#include <Wire.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <DHT.h>
#include <BMP180.h>

// Store an instance of the BMP180 sensor.
BMP180 barometer;                     // BMP180 barometric pressure sensor
RF24 radio(9,10);                     // nRF24L01(+) transceiver aangesloten met pins 9 en 10
RF24Network network(radio);           // netwerk definiëren
DHT dht(2, DHT22);                    // DHT 22 (AM2302) on pin 2 
Sleep sleep;

const unsigned long sleepTime = 50800;// slaaptijd tussen berichten (power saving) 
const uint16_t deze_node = 1;         // adres van onze (sensor) node
const uint16_t andere_node = 0;       // adres van de andere (ontvanger) node
struct sensor_bericht
{
  long p;                             // de luchtdruk
  float t;                            // de temperatuur
  float h;                            // de luchtvochtigheid
};

void setup(void)
{
  Serial.begin(57600);
  SPI.begin();
  radio.begin();
  network.begin(90, deze_node);       // kanaal 90, adres is "deze_node"
  dht.begin();
  Wire.begin();
  barometer = BMP180();
  if(barometer.EnsureConnected())
  {
    barometer.SoftReset();
    barometer.Initialize();
  }
}

void loop(void)
{
  network.update();
  Serial.println("Verzending gegevens...");
  RF24NetworkHeader header(andere_node);    // verstuur de header naar "andere_node"
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  long p = barometer.GetPressure();
  
  sensor_bericht bericht = { p, t, h }; 
  bool ok = network.write(header,&bericht,sizeof(bericht));

   if( ok )
  {
    Serial.println("gelukt.");
  } else
  {
    Serial.println("mislukt.");
  }
  delay(200);
  
  sleep.pwrDownMode();                // stel sleep mode in
  sleep.sleepDelay(sleepTime);        // slaap "sleepTime" ms
}
