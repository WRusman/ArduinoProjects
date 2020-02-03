#include <Servo.h>

Servo lefthip;    // create servo object to control the left hip servo
Servo leftfoot;   // create servo object to control the left foot servo
Servo righthip;   // create servo object to control the right hip servo
Servo rightfoot;  // create servo object to control the right foot servo


void setup() {
  lefthip.attach(5);    // attaches the servo on pin 2 to the left hip servo object
  leftfoot.attach(4);   // attaches the servo on pin 3 to the left foot servo object
  righthip.attach(2);  // attaches the servo on pin 5 to the right hip servo object
  rightfoot.attach(3);  // attaches the servo on pin 4 to the right foot servo object
  lefthip.write(90);    // center the servo
  leftfoot.write(90);    // center the servo
  righthip.write(90);    // center the servo
  rightfoot.write(90);    // center the servo
}


void loop() {
  stepBackward(10);
  delay(10000);
}

void stepForward(int steps)
{
  moveServo(rightfoot,90,120);
  moveServo(leftfoot,90,130);

  moveServo(lefthip,90,120);
  moveServo(righthip,90,120);

  for (int x=0; x <= steps; x++){

  moveServo(leftfoot,130,90);
  moveServo(rightfoot,120,90);
  moveServo(leftfoot,90,60);
  moveServo(rightfoot,90,50);
  
  moveServo(righthip,120,60);
  moveServo(lefthip,120,60);
    
  moveServo(rightfoot,50,90);
  moveServo(leftfoot,60,90);
  moveServo(rightfoot,90,120);
  moveServo(leftfoot,90,130);

  moveServo(lefthip,60,120);
  moveServo(righthip,60,120);
  }

  moveServo(leftfoot,130,90);
  moveServo(righthip,120,90);
  moveServo(lefthip,120,90);
  moveServo(rightfoot,120,90);

}

void stepBackward(int steps)
{
  moveServo(rightfoot,90,60);
  moveServo(leftfoot,90,50);

  moveServo(lefthip,90,60);
  moveServo(righthip,90,60);

  for (int x=0; x <= steps; x++){

  moveServo(leftfoot,50,90);
  moveServo(rightfoot,60,90);
  moveServo(leftfoot,90,120);
  moveServo(rightfoot,90,130);
  
  moveServo(righthip,60,120);
  moveServo(lefthip,60,120);
    
  moveServo(rightfoot,130,90);
  moveServo(leftfoot,120,90);
  moveServo(rightfoot,90,60);
  moveServo(leftfoot,90,50);

  moveServo(lefthip,120,60);
  moveServo(righthip,120,60);
  }

  moveServo(leftfoot,50,90);
  moveServo(righthip,60,90);
  moveServo(lefthip,60,90);
  moveServo(rightfoot,60,90);

}


void moveServo(Servo servoName, int oldAngle, int newAngle)
{
  if (newAngle > oldAngle){
    for (int x=oldAngle; x <= newAngle; x++)
    {
      servoName.write(x); 
      delay(10);
    }
  }
  if (newAngle < oldAngle){
    for (int x=oldAngle; x >= newAngle; x--)
    {
      servoName.write(x);
      delay(10); 
    }
  }
  
}
