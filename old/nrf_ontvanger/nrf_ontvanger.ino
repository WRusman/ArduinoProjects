#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
 
// we definieeren de structuur van het te ontvangen bericht (het envelopje met daarin de gegevens)
struct sensor_bericht
{
  long p;                  // de luchtdruk
  float t;                 // de temperatuur
  float h;                 // de luchtvochtigheid 
};
 
RF24 radio(9,10);             // nRF24L01(+) transceiver aangesloten met pins 9 en 10
RF24Network network(radio);   // netwerk definiëren
 
const uint16_t deze_node = 0;   // adres van onze (ontvanger) node
 
void setup(void)
{
  Serial.begin(57600);
 
  SPI.begin();
  radio.begin();
  network.begin(/*kanaal*/ 90, /*node adres*/ deze_node);
  
  Serial.println("Sensor netwerk ontvanger start...");  
}
 
void loop(void)
{
  network.update();
  while ( network.available() )
  {
    RF24NetworkHeader header;
    sensor_bericht bericht; 
    network.read(header,&bericht,sizeof(bericht));

    Serial.print(" Temperatuur: ");
    Serial.print(bericht.t);
    Serial.print(" Luchtvochtigheid: ");
    Serial.print(bericht.h);
    Serial.print(" Luchtdruk: ");
    Serial.println(bericht.p); 
    
  }
}
