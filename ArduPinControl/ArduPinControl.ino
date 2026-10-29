#include <Joystick.h>   //https://github.com/MHeironimus/ArduinoJoystickLibrary/wiki
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
Joystick_ Joystick(0x03,0x04,13,0,true,true,false,false,false,false,false,false,false,false,false);

float xbaseline, ybaseline, zbaseline;
sensors_event_t a, g, temp;
int inputPins[13]= {4,5,6,8,9,10,14,15,16,18,19,20,21};
int buttonState[13]= {0,0,0,0,0,0,0,0,0,0,0,0,0};
int lastButtonState[13]= {0,0,0,0,0,0,0,0,0,0,0,0,0};
int outButton[13]= {0,1,2,3,4,5,6,7,8,9,10,11,12};
unsigned long lastDebounceTime[13];
unsigned long debounceDelay = 3;

void setup() {
  for (int x=0; x< sizeof(inputPins); x++){
    pinMode(inputPins[x], INPUT_PULLUP);
    }
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);

  Joystick.begin();
  Joystick.setXAxisRange(-512, 512);  
  Joystick.setYAxisRange(-512, 512);  
  Joystick.setXAxis(0);    
  Joystick.setYAxis(0);

  for (int i = 0; i < 10; i++) {
    mpu.getEvent(&a, &g, &temp);
    xbaseline += a.acceleration.x;
    ybaseline += a.acceleration.y;
    zbaseline += a.acceleration.z;
  }
  xbaseline /= 10;
  ybaseline /= 10;
  zbaseline /= 10;
  
}

void loop() {
  mpu.getEvent(&a, &g, &temp);
  Joystick.setXAxis(map(a.acceleration.y - xbaseline, -2, 2, 512, -512));
  Joystick.setYAxis(map(a.acceleration.x - ybaseline, -2, 2, 512, -512));
      
  for (int x=0; x< 13; x++){
    int currentButtonState = !digitalRead(inputPins[x]);
    if (currentButtonState != lastButtonState[x])
    {
      lastDebounceTime[x] = millis();
    }

    if ((millis() - lastDebounceTime[x]) > debounceDelay) {
      Joystick.setButton(outButton[x], currentButtonState);
    }
    lastButtonState[x] = currentButtonState;
  }
}
