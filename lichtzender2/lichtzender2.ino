
#include <NewRemoteTransmitter.h>

NewRemoteTransmitter transmitter(1234567, 13, 260, 3);

bool outputState1=LOW,outputState2=LOW, outputState3=LOW, outputState4=LOW, outputState5=LOW, outputState6=LOW, outputState7=LOW, outputState8=LOW;
bool switch1=HIGH, switch2=HIGH, switch3=HIGH, switch4=HIGH, switch5=HIGH, switch6=HIGH, switch7=HIGH, switch8=HIGH;

void setup() {
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(13,OUTPUT);
}

void loop() {

 switch1=digitalRead(2); 
 if ((switch1 == LOW ) and (outputState1==LOW)){
  transmitter.sendUnit(1, true);
  outputState1=HIGH;
 }
 if ((switch1 == HIGH ) and (outputState1==HIGH)){
  transmitter.sendUnit(1, false);
  outputState1=LOW;
 }

 switch2=digitalRead(3); 
 if ((switch2 == LOW ) and (outputState2==LOW)){
  transmitter.sendUnit(2, true);
  outputState2=HIGH;
 }
 if ((switch2 == HIGH ) and (outputState2==HIGH)){
  transmitter.sendUnit(2, false);
  outputState2=LOW;
 }
 
 switch3=digitalRead(4); 
 if ((switch3 == LOW ) and (outputState3==LOW)){
  transmitter.sendUnit(3, true);
  outputState3=HIGH;
 }
 if ((switch3 == HIGH ) and (outputState3==HIGH)){
  transmitter.sendUnit(3, false);
  outputState3=LOW;
 }
 
 switch4=digitalRead(5); 
 if ((switch4 == LOW ) and (outputState4==LOW)){
  transmitter.sendUnit(4, true);
  outputState4=HIGH;
 }
 if ((switch4 == HIGH ) and (outputState4==HIGH)){
  transmitter.sendUnit(4, false);
  outputState4=LOW;
 }
 
 switch5=digitalRead(6); 
 if ((switch5 == LOW ) and (outputState5==LOW)){
  transmitter.sendUnit(5, true);
  outputState5=HIGH;
 }
 if ((switch5 == HIGH ) and (outputState5==HIGH)){
  transmitter.sendUnit(5, false);
  outputState5=LOW;
 }
 
 switch6=digitalRead(7); 
 if ((switch6 == LOW ) and (outputState6==LOW)){
  transmitter.sendUnit(6, true);
  outputState6=HIGH;
 }
 if ((switch6 == HIGH ) and (outputState6==HIGH)){
  transmitter.sendUnit(6, false);
  outputState6=LOW;
 }
 
 switch7=digitalRead(8); 
 if ((switch7 == LOW ) and (outputState7==LOW)){
  transmitter.sendUnit(7, true);
  outputState7=HIGH;
 }
 if ((switch7 == HIGH ) and (outputState7==HIGH)){
  transmitter.sendUnit(7, false);
  outputState7=LOW;
 }

 switch8=digitalRead(9); 
 if ((switch8 == LOW ) and (outputState8==LOW)){
  transmitter.sendUnit(8, true);
  outputState8=HIGH;
 }
 if ((switch8 == HIGH ) and (outputState8==HIGH)){
  transmitter.sendUnit(8, false);
  outputState8=LOW;
 }
  
}
