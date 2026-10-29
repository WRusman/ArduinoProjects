#include <SPI.h>

//SPI DATA (MOSI)= D12
//SPI CS (SS) = D10
//SPI Clock (SCL)= A5

int i = 0;
int j = 0;
int strobePin = 7;
int APin = 8;
int BPin = 6;
int CPin = 5;
int CS1Pin = 3;
int CS2Pin = 4;


void setup() {
pinMode(APin, OUTPUT); // 74HC238 Data A
pinMode(BPin, OUTPUT); // 74HC238 Data B
pinMode(CPin, OUTPUT); // 74HC238 Data C
pinMode(CS1Pin, OUTPUT); // 74HC238 Chip Select
pinMode(CS2Pin, OUTPUT); // 74HC238 Chip Select
pinMode(10, OUTPUT); // SPI CS
pinMode(11, OUTPUT); // SPI MOSI
pinMode(13, OUTPUT); // SPI CLK
pinMode(strobePin, OUTPUT); // Strobe signaal voor alle TPIC6B595's
digitalWrite(strobePin, LOW);

SPI.begin();
SPI.setClockDivider(16); // SPI clock op 1/16 snelheid is snel zat.
SPI.setDataMode(0); // Data inklokken op opgaande flank van SCK
}
 
void loop() {
  i++; // Teller van de led rijen ophogen
  i%=16; // Teller resetten bij 16
  digitalWrite(APin, bitRead(i,0)); // 74HC238 Data
  digitalWrite(BPin, bitRead(i,1)); // 74HC238 Data
  digitalWrite(CPin, bitRead(i,2)); // 74HC238 Data
  digitalWrite(CS1Pin, bitRead(i,3)); // 74HC238 Select
  digitalWrite(CS2Pin, !bitRead(i,3)); // 74HC238 Select
  
  for(j=0; j<8; j++){ // Alle 24 de TPIC6B595's om beurten vullen met data
    
    SPI.transfer(random(255)); // Zend 8 bits naar de TPIC6B595's via SPI met Dr aan MOSI en Clk aan SCK
  }
  digitalWrite(strobePin, HIGH); // Klok voor alle 24 de TPIC6B595's de data naar de FETs met het STROBE signaal
  digitalWrite(strobePin, LOW); // Einde klokpuls
}
