#define IRSND_IR_FREQUENCY          38000
#define IRSND_OUTPUT_PIN 4           // IR LED = attiny pin 3
#define IRMP_PROTOCOL_NAMES 1
#define IRMP_SUPPORT_JVC_PROTOCOL   
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <irsndSelectAllProtocols.h>     
#include <irsnd.c.h>

IRMP_DATA irsnd_data;

void setup()
{
  irsnd_init();
  pinMode(4,OUTPUT); // IRLED
  pinMode(0,INPUT_PULLUP); // power button
  pinMode(1,INPUT_PULLUP); // volume up button
  pinMode(2,INPUT_PULLUP); // volume down button
  pinMode(3,INPUT_PULLUP); // source button
 
}

void loop() {
  if (digitalRead(0) == LOW)           // 0 = attiny pin 5 
     sendIR(1);
  else if (digitalRead(1) == LOW)       // 1 = attiny pin 6 
     sendIR(2);
  else if (digitalRead(2) == LOW)       // 2 = attiny pin 7 
     sendIR(3);
  else if (digitalRead(3) == LOW)       // 3 = attiny pin 2 
     sendIR(4);
  else
  {}
    //nothing   
}

void sendIR(int IRcommand){
  irsnd_data.protocol = IRMP_JVC_PROTOCOL;
  irsnd_data.address = 0x3;
  
  switch (IRcommand){
    case 1: 
      irsnd_data.command = 0x170;
      break;
    case 2: 
      irsnd_data.command = 0x1E0;
      break;
    case 3: 
      irsnd_data.command = 0x1F0;
      break;
    case 4: 
      irsnd_data.command = 0x1A0;
      break;
    }
  
  irsnd_data.flags = 2;
  irsnd_send_data(&irsnd_data, true); // true = wait for frame to end
}
