#include <BleKeyboard.h>
#include <ESP_FlexyStepper.h>

//switch connected to connected to pin 1
//step pin connected to pin 2
//dir pin connected to gnd

const int MOTOR_STEP_PIN = 2;
const int MOTOR_DIRECTION_PIN = 3;

const int stepsPerRevolution = 3200;
const int numberOfPhotos = 36;

BleKeyboard bleKeyboard("3dScanShutter", "Maakplek", 100);
ESP_FlexyStepper stepper;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE keyboard");
  pinMode(1,INPUT_PULLUP);
  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);    
  bleKeyboard.begin();
  stepper.setSpeedInStepsPerSecond(100);
  stepper.setAccelerationInStepsPerSecondPerSecond(100);
}

void loop() {
  int switchstate = digitalRead(1);
  if((bleKeyboard.isConnected()) and (switchstate==0)) {
      Serial.println("Rotating...");
      stepper.moveRelativeInSteps(stepsPerRevolution);
      Serial.println("klik !");
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      Serial.println("Waiting 5 seconds...");
      delay(5000);
  }
}
