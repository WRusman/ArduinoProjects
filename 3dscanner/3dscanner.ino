#include <BleKeyboard.h>

//switch connected to pin 1
//step pin connected to pin 2
//dir pin connected to gnd

const int MOTOR_STEP_PIN = 2;

const int stepsPerRevolution = 3200;
const int numberOfPhotos = 36;

BleKeyboard bleKeyboard("3dScanShutter", "Maakplek", 100);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE keyboard");
  pinMode(1,INPUT_PULLUP);
  bleKeyboard.begin();
}

void loop() {
  int switchstate = digitalRead(1);
  if((bleKeyboard.isConnected()) and (switchstate==0)) {
      Serial.println("Rotating...");
      for (int x=0; x < stepsPerRevolution; x++){
        digitalWrite(MOTOR_STEP_PIN,1);
        delay(10);
        digitalWrite(MOTOR_STEP_PIN,0);
        delay(10);
      }
      Serial.println("klik !");
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      Serial.println("Waiting 5 seconds...");
      delay(5000);
  }
}
