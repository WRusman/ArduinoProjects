#include <CapacitiveSensor.h>

/*
 * Capacitive sensor switch for mirror lighting
 */


CapacitiveSensor   sensor = CapacitiveSensor(4,2);  // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
const int FETPin = 9;                               // pin that the FET is attached to
int lightState = 0;
int triggerCount = 0;
long previousValue = 0L;

void setup()                    
{
  sensor.set_CS_AutocaL_Millis(0xFFFFFFFF);
  Serial.begin(9600);
  pinMode (FETPin, OUTPUT);
}

void loop()                    
{
    long sensorValue =  sensor.capacitiveSensor(80);
    if (sensorValue >100L && previousValue > 100L){
       triggerCount++;
    } 
    else {
      triggerCount = 0;
    }
    
    if (triggerCount == 20){
      if (lightState == 0){
        for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) { 
          analogWrite(FETPin, fadeValue);         
          delay(10);          
        } 
        lightState=1;
        delay(2000);
      }
      else
      {
       for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) { 
        analogWrite(FETPin, fadeValue);         
        delay(10);                            
        }
        lightState=0;
        delay(2000);
      }
      sensor.reset_CS_AutoCal();
      triggerCount = 0;
    }

     previousValue = sensorValue;     
    Serial.println(sensorValue);                  // print sensor output 1
    Serial.println(triggerCount);
    //delay(30);                             // arbitrary delay to limit data to serial port 
}
