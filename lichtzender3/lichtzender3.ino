
#include <NewRemoteTransmitter.h>

NewRemoteTransmitter transmitter(1234567, 13, 260, 3);

bool outputState[8] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};
bool switchState[8] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};

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

for (int switchNumber=1;switchNumber<=8;switchNumber++){  

   switchState[switchNumber-1]=digitalRead((switchNumber+1)); 

   if ((outputState[switchNumber-1]==LOW) and (switchState[switchNumber-1] == LOW )){
      transmitter.sendUnit(switchNumber, true);
      outputState[switchNumber]=HIGH;
     }

   if ((outputState[switchNumber-1]==HIGH) and (switchState[switchNumber-1] == HIGH )){
      transmitter.sendUnit(switchNumber, false);
      outputState[switchNumber]=LOW;
     }
  }
}
