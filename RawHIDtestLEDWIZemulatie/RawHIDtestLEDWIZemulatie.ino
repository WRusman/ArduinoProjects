/*
  Koen Delvaux's way to emulate a LedWiz with an Arduino Pro Micro
  Output is 18 ports, the rest is ignored
  
  To install:
  - install the Arduino Sparkfun AVR boards definitions from within the Arduino IDE
  - install the HID-project library from https://github.com/NicoHood/HID/wiki/System-API
  - install the Arduino-SoftPWM library from https://github.com/Palatis/arduino-softpwm

  IMPORTANT
  For this to work, you need to change some stuff on the USB-HID config side
  -> in the file board.txt, e.g. located on C:\Users\[username]\AppData\Local\Arduino15\packages\SparkFun\hardware\avr\1.1.13
  -> in the file RawHID.h e.g. located on C:\Users\[username]\Documents\Arduino\libraries\HID-Project\src 

  Modifications for boards.txt (restart Arduino IDE after schanging this)

  promicro.build.usb_product="USB LED-Wiz Copy by Koen"
  promicro.build.vid=0xFAFA
  promicro.build.pid=0x00F0
  promicro.menu.cpu.16MHzatmega32U4.build.pid=0x00F0
  also add the following line::
  promicro.build.pid=0x00F0

  Modifications for RawHID.h

  #undef RAWHID_USAGE_PAGE
  #define RAWHID_USAGE_PAGE	65280 // recommended: 0xFF00 to 0xFFFF

  #undef RAWHID_USAGE
  #define RAWHID_USAGE		0x0001 // recommended: 0x0100 to 0xFFFF

  // Keep one byte offset for the reportID if used
  #if (HID_REPORTID_RAWHID)
  #define RAWHID_SIZE (8) // USB_EP_SIZE-1 of 8
  #error RAWHID does not work properly with a report ID and multiple reports.
  #error Please remove this manually if you know what you are doing.
  #else
  #define RAWHID_SIZE (8) // USB_EP_SIZE of 8
  #endif

  If you have problems after building, remove the VID en PID from the entry in the registry with regedit.exe
*/
#define HAVE_HWSERIAL0
#include "HID-Project.h"
#include <SoftPWM.h>
#define HAVE_HWSERIAL0 true

SOFTPWM_DEFINE_CHANNEL(0, DDRD, PORTD, PORTD3);  //Arduino pin met usb bovenaan linksboven
SOFTPWM_DEFINE_CHANNEL(1, DDRD, PORTD, PORTD2);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(2, DDRD, PORTD, PORTD1);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(3, DDRD, PORTD, PORTD0);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(4, DDRD, PORTD, PORTD4);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(5, DDRC, PORTC, PORTC6);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(6, DDRD, PORTD, PORTD7);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(7, DDRE, PORTE, PORTE6);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(8, DDRB, PORTB, PORTB4);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(9, DDRB, PORTB, PORTB5);  //Arduino pin met usb bovenaan linksboven
SOFTPWM_DEFINE_CHANNEL(10, DDRB, PORTB, PORTB6);  //Arduino pin met usb bovenaan rechtsonder
SOFTPWM_DEFINE_CHANNEL(11, DDRB, PORTB, PORTB2);  //Arduino pin
SOFTPWM_DEFINE_CHANNEL(12, DDRB, PORTB, PORTB3);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(13, DDRB, PORTB, PORTB1);  //Arduino pin
SOFTPWM_DEFINE_CHANNEL(14, DDRF, PORTF, PORTF7);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(15, DDRF, PORTF, PORTF6);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(16, DDRF, PORTF, PORTF5);  //Arduino pin 
SOFTPWM_DEFINE_CHANNEL(17, DDRF, PORTF, PORTF4);  //Arduino pin met usb bovenaan rechtsboven

SOFTPWM_DEFINE_OBJECT_WITH_PWM_LEVELS(18, 256);

// Gamma correction table for 8-bit input values
static const uint8_t dof_to_gamma_8bit[] = {
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1, 
      1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2, 
      2,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   5,   5,   5, 
      5,   6,   6,   6,   6,   7,   7,   7,   7,   8,   8,   8,   9,   9,   9,  10, 
     10,  10,  11,  11,  11,  12,  12,  13,  13,  13,  14,  14,  15,  15,  16,  16, 
     17,  17,  18,  18,  19,  19,  20,  20,  21,  21,  22,  22,  23,  24,  24,  25, 
     25,  26,  27,  27,  28,  29,  29,  30,  31,  32,  32,  33,  34,  35,  35,  36, 
     37,  38,  39,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  50, 
     51,  52,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  66,  67,  68, 
     69,  70,  72,  73,  74,  75,  77,  78,  79,  81,  82,  83,  85,  86,  87,  89, 
     90,  92,  93,  95,  96,  98,  99, 101, 102, 104, 105, 107, 109, 110, 112, 114, 
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142, 
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175, 
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213, 
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};
// a map to translate the ledwiz 0-48 brightness values to 0-255. Added an additional entry for 49, which is also max
static const uint8_t map_brightness[] = {
    0, 5,  10,  15,  21,  26,  31,  37,  42,  47,  53,  58,  63,  69,  74,  79,  85,  
    90,  95,  100,  106,  111,  116,  122,  127,  132,  138,  143,  148,  154,  159,  
    164,  170,  175,  180,  185,  191,  196,  201,  207,  212,  217,  223,  228,  233,  
    239,  244,  249,  255, 255
};

// Buffer to hold RawHID data.
// If host tries to send more data than this,
// it will respond with an error.
// If the data is not read until the host sends the next data
// it will also respond with an error and the data will be lost.
uint8_t rawhidData[255];
// global variables
static volatile uint8_t counter = 0;
static volatile long timeToChangeLevels = 0;
static volatile uint8_t LED_values[32];
static volatile bool LED_enabled[32]= {false};
static volatile uint8_t current_bank = 0;
static volatile uint8_t time_multiplier = 7;

// initialise the PWM and the HID interface
void setup() {
  Serial.begin(115200);
 Serial.println("Init program done");
  // initialise PWM 
    Palatis::SoftPWM.begin(60);
  // Set the RawHID OUT report array.
  // Feature reports are also (parallel) possible, see the other example for this.
  RawHID.begin(rawhidData, sizeof(rawhidData));
}



void loop() {

  if (micros() > timeToChangeLevels) {
    // Pre-calculate the levels for the automated sequences 
    // we directly apply the gamma mapping to avoid having to do this lookup multiple times at runtime
    uint8_t triangle; // bitmode 129
    uint8_t rectangle; // bitmode 130
    uint8_t fall; // bitmode 131
    uint8_t rise; // bitmode 132
    if (counter < 128) {
      triangle= dof_to_gamma_8bit[counter*2];
      rectangle= dof_to_gamma_8bit[255]; 
    }
    else {
      triangle= dof_to_gamma_8bit[(255-counter)*2];
      rectangle= dof_to_gamma_8bit[0];
    }
    fall= dof_to_gamma_8bit[255-counter];
    rise= dof_to_gamma_8bit[counter];

    // Now we go and update the brightness of all of our LEDs
    // On Arduino Pro Micro we only have 18 ports, so ignore the rest
    for (int i=0; i<18;i++) {
      if (LED_enabled[i]) {
        // allright, the LED is ENABLED so we can look up the value is should have
        if (LED_values[i]<50) // This is a fixed value. We also include 49 since LedWiz also accepts it.
          // we first map the value from the LEDWiz 0-48 range to the 0-256 range we use
          // then we apply a gamma corretion map
          Palatis::SoftPWM.set(i, dof_to_gamma_8bit[map_brightness[LED_values[i]]] );
        else if (LED_values[i]==129) // now we need to set the triangle value
          Palatis::SoftPWM.set(i, triangle );
        else if (LED_values[i]==130) // now we need to set the rectangle value
          Palatis::SoftPWM.set(i, rectangle );
        else if (LED_values[i]==131) // now we need to set the fall value
          Palatis::SoftPWM.set(i, fall );
        else if (LED_values[i]==132) // now we need to set the rise value
          Palatis::SoftPWM.set(i, rise );
        else                        // this should not happen, just use 0
          Palatis::SoftPWM.set(i, 0 );        
      }
      else {
        // no matter what the value is, if it is DISABLED we have to switch it off
        Palatis::SoftPWM.set(i, 0 );
      }
    }
    // Get ready for the next round (counter will rollover from 255 to 0 if needed)
    //if (counter == 0) Palatis::SoftPWM.printInterruptLoad();
    // speed 1 is 250ms per full cycle, and speed 7 is 7 times slower.
    // To wrap around in 256 times in 250ms, each round needs to be 977ms on speed 1
    // to move slower, simply wait longer to move to the next level
    timeToChangeLevels= micros()+(977*time_multiplier);
    counter++;
  }
   
  //normal serial operation
  auto bytesAvailable = RawHID.available();
  if (bytesAvailable == 8) {
    // process the message
    uint8_t received= RawHID.read();
    if (received == 64) {
      // this updates the on/off state and the speed of the automatic patterns
      for (int i=0; i<4;i++) {
        // read the next byte
        received= RawHID.read();
        // assign it to the ports in this byte, 8 bits show the on/off status
        for (int port=0;port<8;port++) {
          LED_enabled[i*8+port] = (1 == (received & 1));
          // DEBUG
          /*
          Serial.print("enabled: ");
          Serial.print(i*8+port);
          Serial.print("= ");
          Serial.print(received & 1);
          Serial.println();
          */
          received >>= 1;
        }        
      }
      // some more reading to do. There are 3 more bytes
      received= RawHID.read();
      // the first byte is a multiplier for the speeds of the automatic patterns, going from 1-7
      if ((received > 0) && (received < 8)) time_multiplier= received;
      // DEBUG
      /*
      Serial.print("Speed: ");
      Serial.print(time_multiplier);
      Serial.println();
      Serial.println();
      */
      // the next two bytes are unused
      received= RawHID.read();
      received= RawHID.read();
      // after receiving an on/off message, we also have to reset the bank number for receiving brightness/mode
      current_bank = 0;       
    }
    else {
      // so this first byte we received is already the brightness or mode for the first port in this bank
      LED_values[current_bank * 8]= received;
      // DEBUG
      /*
      Serial.print("Value: ");
      Serial.print(current_bank * 8);
      Serial.print("= ");
      Serial.print(received);
      Serial.println();
      */
      // but there is 7 more bytes to read and set the port value
      for (int i=1; i<8;i++) {
        // each byte defines either the brightness or the mode
        received = RawHID.read();
        LED_values[current_bank * 8 + i] = received;
        // DEBUG
        /*
        Serial.print("Value: ");
        Serial.print(current_bank * 8 + i);
        Serial.print("= ");
        Serial.print(received);
        Serial.println();
        */
      }
      // DEBUG 
      /*
      LED_values[0] = 129;
      LED_values[1] = 130;
      LED_values[2] = 132;
      Serial.println();
      */
      // next time, listen to the next bank number
      current_bank++;
      // don't forget to wrap around: there's only 4 banks
      if (current_bank == 4) current_bank = 0;
    }
  }
  else {
    // unexpected message, print it
    while (bytesAvailable--) {
    // DEBUG 
    /*
    Serial.print("Unexpected HID message: ");
    Serial.print(RawHID.read());
    Serial.println();
    */
    }
  }
}
