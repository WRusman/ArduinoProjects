#include <Encoder.h>
#include <Keyboard.h>

Encoder myEnc(8,9);
const int input_x=2; 
const int input_y=3; 
const int input_z=4; 
const int input_a=5; 
const int input_m1=21; 
const int input_m2=20; 
const int input_m3=19;
long oldPosition, newPosition;
char activeAxis;
int stepMultiplier;

void setup() {
  Keyboard.begin();
  Serial.begin(9600);
  Serial.println("Basic Pendant:");
  pinMode(input_x, INPUT_PULLUP);
  pinMode(input_y, INPUT_PULLUP);
  pinMode(input_z, INPUT_PULLUP);  
  pinMode(input_a, INPUT_PULLUP);  
  pinMode(input_m1, INPUT_PULLUP);  
  pinMode(input_m2, INPUT_PULLUP);  
  pinMode(input_m3, INPUT_PULLUP);    
}

void loop() {
  newPosition = myEnc.read();
      
  if (newPosition < oldPosition ) {
   
    Serial.println("x+");    
    
    
//    Keyboard.press(KEY_LEFT_SHIFT);Keyboard.press(KEY_KP_4);Keyboard.releaseAll();
    
  }
  else if (newPosition > oldPosition){
    Serial.println("x-");    
  }
  oldPosition = newPosition;
}

void readSwitches() {
  if (digitalRead(input_x) == 0) activeAxis="x";          // axis X active
  else if (digitalRead(input_y) == 0) activeAxis="y";     // axis Y active  
  else if (digitalRead(input_z) == 0) activeAxis="z";     // axis Z active  
  else if (digitalRead(input_a) == 0) activeAxis="a";     // axis A active  
  else activeAxis="n";    

  if (digitalRead(input_m1) == 0) stepMultiplier=1;       // fast!
  else if (digitalRead(input_m2) == 0) stepMultiplier=2;  // normal
  else stepMultiplier=2;                                  // slow


}
