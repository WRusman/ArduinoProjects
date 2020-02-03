#include <Servo.h>

Servo lefthip;    // create servo object to control the left hip servo
Servo leftfoot;   // create servo object to control the left foot servo
Servo righthip;   // create servo object to control the right hip servo
Servo rightfoot;  // create servo object to control the right foot servo


void setup() {
  lefthip.attach(2);    // attaches the servo on pin 2 to the left hip servo object
  leftfoot.attach(3);   // attaches the servo on pin 3 to the left foot servo object
  righthip.attach(5);  // attaches the servo on pin 5 to the right hip servo object
  rightfoot.attach(4);  // attaches the servo on pin 4 to the right foot servo object
  lefthip.write(90);    // center the servo
  leftfoot.write(90);    // center the servo
  righthip.write(90);    // center the servo
  rightfoot.write(90);    // center the servo
}


void loop() {

}
