#define IRSND_IR_FREQUENCY          38000
#define IRSND_OUTPUT_PIN 4           // IR LED = attiny pin 3
#define IRMP_PROTOCOL_NAMES 1
#define IRMP_SUPPORT_JVC_PROTOCOL   
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <irsndSelectAllProtocols.h>     
#include <irsnd.c.h>

#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)

ISR(PCINT0_vect) {
}

IRMP_DATA irsnd_data;

void setup()
{
  pinMode(4,OUTPUT); // IRLED
  pinMode(0,INPUT_PULLUP); // power button
  pinMode(1,INPUT_PULLUP); // volume up button
  pinMode(2,INPUT_PULLUP); // volume down button
  pinMode(3,INPUT_PULLUP); // source button
  irsnd_init();
}

void loop() {
  sleep();
  
  if (digitalRead(0) == LOW)           // PB0 = attiny pin 5 
     sendIR(1);
  else if (digitalRead(1) == LOW)       // PB1 = attiny pin 6 
     sendIR(2);
  else if (digitalRead(2) == LOW)       // PB2 = attiny pin 7 
     sendIR(3);
  else if (digitalRead(3) == LOW)       // PB2 = attiny pin 2 
     sendIR(4);
}

void sleep() {

    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT0);                   // Use PB0 as interrupt pin
    PCMSK |= _BV(PCINT1);                   // Use PB1 as interrupt pin
    PCMSK |= _BV(PCINT2);                   // Use PB2 as interrupt pin
    PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
   
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
    } // sleep

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
      irsnd_data.command = 0x130;
      break;
    
    }
  
  irsnd_data.flags = 2;
  irsnd_send_data(&irsnd_data, true); // true = wait for frame to end
}
