#include <Thread.h>
#include <ThreadController.h>
#include <Servo.h>
#include <Wire.h>
#include "person_sensor.h"
#include "defines.h"
#include "functions.h"

void setup(){
	Serial.begin(115200);
  Wire.begin();
  randomSeed(analogRead(A15));

	eye1Thread->onRun(eye1Move);
  eye2Thread->onRun(eye2Move);
  eye3Thread->onRun(eye3Move);
  eye4Thread->onRun(eye4Move);
  eye5Thread->onRun(eye5Move);
  eye6Thread->onRun(eye6Move);
  eye7Thread->onRun(eye7Move);

  eye1BlinkThread->onRun(eye1Blink);
  eye2BlinkThread->onRun(eye2Blink);
  eye3BlinkThread->onRun(eye3Blink);
  eye4BlinkThread->onRun(eye4Blink);
  eye5BlinkThread->onRun(eye5Blink);
  eye6BlinkThread->onRun(eye6Blink);
  eye7BlinkThread->onRun(eye7Blink);

  personSensorThread->onRun(personSensorCallback);
  personSensorThread->setInterval(200);

  eye1Thread->setInterval(random(3000,7000));
  eye2Thread->setInterval(random(3000,7000));
  eye3Thread->setInterval(random(3000,7000));
  eye4Thread->setInterval(random(3000,7000));
  eye5Thread->setInterval(random(3000,7000));
  eye6Thread->setInterval(random(3000,7000));
  eye7Thread->setInterval(random(3000,7000));

  eye1BlinkThread->setInterval(random(3000,8000));
  eye2BlinkThread->setInterval(random(3000,8000));
  eye3BlinkThread->setInterval(random(3000,8000));
  eye4BlinkThread->setInterval(random(3000,8000));
  eye5BlinkThread->setInterval(random(3000,8000));
  eye6BlinkThread->setInterval(random(3000,8000));
  eye7BlinkThread->setInterval(random(3000,8000));

          
	controll.add(eye1Thread);
  controll.add(eye2Thread);
  controll.add(eye3Thread);
  controll.add(eye4Thread);
  controll.add(eye5Thread);
  controll.add(eye6Thread);
  controll.add(eye7Thread);
  controll.add(eye1BlinkThread);
  controll.add(eye2BlinkThread);
  controll.add(eye3BlinkThread);
  controll.add(eye4BlinkThread);
  controll.add(eye5BlinkThread);
  controll.add(eye6BlinkThread);
  controll.add(eye7BlinkThread);
  controll.add(personSensorThread);

  attachServos();
}

void loop(){
	controll.run();
}
