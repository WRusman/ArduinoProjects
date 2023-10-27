#include <ESP32Servo.h>

Servo servo1;


void setup() {
servo1.setPeriodHertz(50);    // standard 50 hz servo
servo1.attach(4, 1000, 2000); // attaches the servo on pin 18 to the servo object

}

void loop() {
servo1.write(90);

}
