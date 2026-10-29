
#include <NewRemoteTransmitter.h>

NewRemoteTransmitter transmitter(1234567, 13, 260, 3);

void setup() {
  pinMode(13,OUTPUT);
}

void loop() {
  transmitter.sendGroup(false);
  delay(10000);  
}
