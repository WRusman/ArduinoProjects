/*
 * This code contains the follow functions:
 * - void setup(): Sets pins 4, 5, 6, 7 to input with pull-up resistors enabled and begins Keyboard functionality
 * - void loop(): Main loop - reads pin voltages and sends out corresponding keystrokes via USB
 */

/* 
 * Pinout:
 * - "HIGH" voltage button contacts - pins 4, 5, 6, 7
 * - "GND" voltage button contacts - GND pin
 */

#include <Keyboard.h>

int escStatus=1, escStatusPrev=1;
int enterStatus=1, enterStatusPrev=1;
int upStatus=1, upStatusPrev=1;
int leftStatus=1, leftStatusPrev=1;
int rightStatus=1, rightStatusPrev=1;
int downStatus=1, downStatusPrev=1;



void setup()
{
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  Keyboard.begin();
}

void loop()
{
 
 escStatus=digitalRead(2);
 enterStatus=digitalRead(3);
 upStatus=digitalRead(4);
 leftStatus=digitalRead(5);
 rightStatus=digitalRead(6);
 downStatus=digitalRead(7);

//ESC PRESSED
 if (escStatus!=escStatusPrev && escStatus==LOW)
 {
    Keyboard.press(KEY_ESC);
    escStatusPrev=escStatus;
 }
 //ESC RELEASED
 if (escStatus!=escStatusPrev && escStatus==HIGH)
 {
    Keyboard.release(KEY_ESC);
    escStatusPrev=escStatus;
 }
 
//ENTER PRESSED
 if (enterStatus!=enterStatusPrev && enterStatus==LOW)
 {
    Keyboard.press(KEY_RETURN);
    enterStatusPrev=enterStatus;
 }
 //ENTER RELEASED
 if (enterStatus!=enterStatusPrev && enterStatus==HIGH)
 {
    Keyboard.release(KEY_RETURN);
    enterStatusPrev=enterStatus;
 }

 //UP ARROW PRESSED
 if (upStatus!=upStatusPrev && upStatus==LOW)
 {
    Keyboard.press(KEY_UP_ARROW);
    upStatusPrev=upStatus;
 }
 //UP ARROW RELEASED
 if (upStatus!=upStatusPrev && upStatus==HIGH)
 {
    Keyboard.release(KEY_UP_ARROW);
    upStatusPrev=upStatus;
 }

 //LEFT ARROW PRESSED
 if (leftStatus!=leftStatusPrev && leftStatus==LOW)
 {
    Keyboard.press(KEY_LEFT_ARROW);
    leftStatusPrev=leftStatus;
 }
 //LEFT ARROW RELEASED
 if (leftStatus!=leftStatusPrev && leftStatus==HIGH)
 {
    Keyboard.release(KEY_LEFT_ARROW);
    leftStatusPrev=leftStatus;
 }
 
 //RIGHT ARROW PRESSED
 if (rightStatus!=rightStatusPrev && rightStatus==LOW)
 {
    Keyboard.press(KEY_RIGHT_ARROW);
    rightStatusPrev=rightStatus;
 }

 //RIGHT ARROW RELEASED
 if (rightStatus!=rightStatusPrev && rightStatus==HIGH)
 {
    Keyboard.release(KEY_RIGHT_ARROW);
    rightStatusPrev=rightStatus;
 }

 //DOWN ARROW PRESSED
 if (downStatus!=downStatusPrev && downStatus==LOW)
 {
    Keyboard.press(KEY_DOWN_ARROW);
    downStatusPrev=downStatus;
 }
 //DOWN ARROW RELEASED
 if (downStatus!=downStatusPrev && downStatus==HIGH)
 {
    Keyboard.release(KEY_DOWN_ARROW);
    downStatusPrev=downStatus;
 }

 
}
