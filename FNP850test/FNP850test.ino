#define FNPAddress 0x41
#define faultByte1Reg 0x80
#define faultByte2Reg 0x81
#define vReg 0x8A
#define vScaleReg 0x8C
#define cReg 0x96
#define cScaleReg 0x98
#define fanCtlReg 0xA2
#define vLimitReg 0xA3
#define vLimitScaleReg 0xA5
#define cLimitReg 0xAF
#define cLimitScaleReg 0xB1

#include <Wire.h>

void setup() {
  Wire.begin();
  FNP850FanCTL();
  Serial.begin(9600);

}

void loop() {
  Serial.print("Voltage: ");
  Serial.println(FNP850Voltage());
  Serial.print("Current: ");
  Serial.println(FNP850Current());
  Serial.println();  
  delay(2000);
}

float FNP850Voltage() {
   Wire.beginTransmission(FNPAddress);
   Wire.write(vReg);
   Wire.endTransmission();
   Wire.requestFrom(FNPAddress,2);
   while(Wire.available())
   {
    byte a = Wire.read();
    byte b = Wire.read();
    int voltage = a;
    voltage = voltage << 8 | b;
    return (float(voltage)/ 1000);
   }
}

float FNP850Current() {
   Wire.beginTransmission(FNPAddress);
   Wire.write(cReg);
   Wire.endTransmission();
   Wire.requestFrom(FNPAddress,2);
   while(Wire.available())
   {
    byte a = Wire.read();
    byte b = Wire.read();
    int current = a;
    current = current << 8 | b;
    return (float(current)/ 1000);
   }
}


void FNP850FanCTL(){
   Wire.beginTransmission(FNPAddress);
   Wire.write(fanCtlReg);
   Wire.write(0x00);
   Wire.endTransmission();   
}
