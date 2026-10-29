
#include <RCSwitch.h>

RCSwitch RemoteSwitch = RCSwitch();

bool switchState1=LOW,switchState2=LOW, switchState3=LOW, switchState4=LOW, switchState5=LOW, switchState6=LOW, switchState7=LOW, switchState8=LOW, switchState9=LOW, switchState10=LOW;
bool switch1=HIGH, switch2=HIGH, switch3=HIGH, switch4=HIGH, switch5=HIGH, switch6=HIGH, switch7=HIGH, switch8=HIGH, switch9=HIGH, switch10=HIGH;

void setup() {
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(10,INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(13,OUTPUT);
  RemoteSwitch.enableTransmit(13);
  // Optional set pulse length.
  // mySwitch.setPulseLength(320);

}

void loop() {

 switch1=digitalRead(2); 
 if ((switch1 == LOW ) and (switchState1==LOW)){
  RemoteSwitch.switchOn('a',1,1);
  switchState1=HIGH;
 }
 if ((switch1 == HIGH ) and (switchState1==HIGH)){
  RemoteSwitch.switchOff('a',1,1);
  switchState1=LOW;
 }

 switch2=digitalRead(3); 
 if ((switch2 == LOW ) and (switchState2==LOW)){
  RemoteSwitch.switchOn('a',1,2);
  switchState2=HIGH;
 }
 if ((switch2 == HIGH ) and (switchState2==HIGH)){
  RemoteSwitch.switchOff('a',1,2);
  switchState2=LOW;
 }
 
 switch3=digitalRead(4); 
 if ((switch3 == LOW ) and (switchState3==LOW)){
  RemoteSwitch.switchOn('a',1,3);
  switchState3=HIGH;
 }
 if ((switch3 == HIGH ) and (switchState3==HIGH)){
  RemoteSwitch.switchOff('a',1,3);
  switchState3=LOW;
 }
 
 switch4=digitalRead(5); 
 if ((switch4 == LOW ) and (switchState4==LOW)){
  RemoteSwitch.switchOn('b',1,1);
  switchState4=HIGH;
 }
 if ((switch4 == HIGH ) and (switchState4==HIGH)){
  RemoteSwitch.switchOff('b',1,1);
  switchState4=LOW;
 }
 
 switch5=digitalRead(6); 
 if ((switch5 == LOW ) and (switchState5==LOW)){
  RemoteSwitch.switchOn('b',1,2);
  switchState5=HIGH;
 }
 if ((switch5 == HIGH ) and (switchState5==HIGH)){
  RemoteSwitch.switchOff('b',1,2);
  switchState5=LOW;
 }
 
 switch6=digitalRead(7); 
 if ((switch6 == LOW ) and (switchState6==LOW)){
  RemoteSwitch.switchOn('b', 1, 3);
  switchState6=HIGH;
 }
 if ((switch6 == HIGH ) and (switchState6==HIGH)){
  RemoteSwitch.switchOff('b', 1, 3);
  switchState6=LOW;
 }
 
 switch7=digitalRead(8); 
 if ((switch7 == LOW ) and (switchState7==LOW)){
  RemoteSwitch.switchOn("10000", "00001");
  switchState7=HIGH;
 }
 if ((switch7 == HIGH ) and (switchState7==HIGH)){
  RemoteSwitch.switchOff("10000", "00001");
  switchState7=LOW;
 }
 
 switch8=digitalRead(9); 
 if ((switch8 == LOW ) and (switchState8==LOW)){
  RemoteSwitch.switchOn("10000", "00010");
  switchState8=HIGH;
 }
 if ((switch8 == HIGH ) and (switchState8==HIGH)){
  RemoteSwitch.switchOff("10000", "00010");
  switchState8=LOW;
 }
 
 switch9=digitalRead(10); 
 if ((switch9 == LOW ) and (switchState9==LOW)){
  RemoteSwitch.switchOn("10000", "00011");
  switchState9=HIGH;
 }
 if ((switch9 == HIGH ) and (switchState9==HIGH)){
  RemoteSwitch.switchOff("10000", "00011");
  switchState9=LOW;
 }
 
 switch10=digitalRead(11); 
 if ((switch10 == LOW ) and (switchState10==LOW)){
  RemoteSwitch.switchOn("10000", "00100");
  switchState10=HIGH;
 }
 if ((switch10 == HIGH ) and (switchState10==HIGH)){
  RemoteSwitch.switchOff("10000", "00100");
  switchState10=LOW;
 }
 
  
  
}
