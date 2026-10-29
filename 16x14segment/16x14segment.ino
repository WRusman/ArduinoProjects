//#include "charset.h";

int clockShiftReg = D6 ;//12;  // D6 on the wemos D1 mini
int clockDisplay = D7; //13;   // D7 on the wemos D1 mini
int dataPin = D8; //15;        // D8 on the wemos D1 mini
int brightnessPin = D5; //14;  // D6 on the wemos D1 mini
int enablePin = D3; //0;  // D3 on the wemos D1 mini

void setup() {
  pinMode(clockShiftReg, OUTPUT);
  pinMode(clockDisplay, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(brightnessPin, OUTPUT);
  digitalWrite(clockShiftReg,0);
  digitalWrite(clockDisplay,0);      
  digitalWrite(enablePin,0);      
  analogWrite(brightnessPin,180);    
}

void loop(){
  
 /* digitalWrite(dataPin,1);
  for (int x=0; x<8; x++){
    digitalWrite(clockShiftReg,1);
    delay(20);
    digitalWrite(clockShiftReg,0);    
    delay(20);
  }
  */
  
  digitalWrite(dataPin,1);
  for (int x=0; x<36; x++){
    digitalWrite(dataPin,1);
    digitalWrite(clockDisplay,1);
    delay(20);
    digitalWrite(clockDisplay,0);    
    delay(20);
  }
  digitalWrite(enablePin,1);    
  delay(20);  
  digitalWrite(enablePin,0);    
  
  bool ledstatus = digitalRead(BUILTIN_LED);  
  digitalWrite(BUILTIN_LED,!ledstatus);  
}  
