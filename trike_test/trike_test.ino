#define throttlePin A0    // input of the hall sensor in the throttle handle
#define pwmPin 5          // pwm output to the controller
float pinValue;

void setup() {
  //pinMode(pwmPin,OUTPUT);
  pinMode(throttlePin,INPUT);
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
    pinValue=analogRead(A1);
    //if (pinValue > 1022) {pinValue=1022;}
    //if (pinValue < 0) {pinValue=0;}    
    Serial.print(pinValue);
    Serial.print(" - ");
    Serial.println(map(pinValue,0,1023,0,255));
    analogWrite(pwmPin,map(pinValue,0,1023,0,255));
    delay(100);
}


  
