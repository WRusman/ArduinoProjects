/*
  The circuit:
  - pushbuttons attached from pin to Gnd
*/

#include "Keyboard.h"

#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty

// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
byte buttons[] = {0, 1, 2, 3, 9, 10, 11, 14, 15, 16}; // the analog 0-5 pins are also known as 14-19
// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)
// we will track if a button is just pressed, just released, or 'currently pressed' 
volatile byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];

void setup() {
  byte i;
  
  // Make input & enable pull-up resistors on switch pins
  for (i=0; ipinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  }

  // Run timer2 interrupt every 15 ms 
  TCCR2A = 0;
  TCCR2B = 1<<CS22 | 1<<CS21 | 1<<CS20;

  //Timer2 Overflow Interrupt Enable
  TIMSK2 |= 1<<TOIE2;

}

SIGNAL(TIMER2_OVF_vect) {
  check_switches();
}

void check_switches()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;

  if (millis() // we wrapped around, lets just try again
     lasttime = millis();
  }
  
  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  
  for (index = 0; index digitalRead(buttons[index]);   // read the button
    
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
          // just pressed
          justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
          // just released
          justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }
    //Serial.println(pressed[index], DEC);
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}


void loop() {
  for (byte i = 0; i if (justpressed[i]) {
      justpressed[i] = 0;
      //Serial.print(i, DEC);
      //Serial.println(" Just pressed"); 
      // remember, check_switches() will CLEAR the 'just pressed' flag
    }
    if (justreleased[i]) {
      justreleased[i] = 0;
      // remember, check_switches() will CLEAR the 'just pressed' flag
    }
    if (pressed[i]) {
      // is the button pressed down at this moment
    }
  }
}









int previousButtonState = HIGH;   // for checking the state of a pushButton
int counter = 0;                  // button push counter

void setup() {
  pinMode(0, INPUT_PULLUP); // SELECT
  pinMode(1, INPUT_PULLUP); // START
  pinMode(2, INPUT_PULLUP); // TOP1
  pinMode(3, INPUT_PULLUP); // RIGHT1
  pinMode(9, INPUT_PULLUP); // BOTTOM1
  pinMode(10, INPUT_PULLUP); // LEFT1
  pinMode(11, INPUT_PULLUP); // TOP2
  pinMode(A0, INPUT_PULLUP); // RIGHT2
  pinMode(A1, INPUT_PULLUP); // BOTTOM2
  pinMode(A2, INPUT_PULLUP); // LEFT2

  // initialize control over the keyboard:
  Keyboard.begin();
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  if ((buttonState != previousButtonState)
      && (buttonState == HIGH)) {
    counter++;
    Keyboard.println(" times.");
  }
  // save the current button state for comparison next time:
  previousButtonState = buttonState;
}
