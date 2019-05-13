unsigned long thIn,thOut,stIn,stOut;
#define thInPin 0
#define stInPin 1
#define thOutPin 3
#define stOutPin 4

void setup() {
  pinMode(thInPin,INPUT);
  pinMode(stInPin,INPUT);
  pinMode(thOutPin,OUTPUT);
  pinMode(stOutPin,OUTPUT);
}

void loop() {
  thIn = pulseIn(thInPin, HIGH);
  thOut = (thIn / 2);
  writePWM(thOutPin,thOut); 
  stIn = pulseIn(stInPin, HIGH);
  stOut = (stIn / 2);
  writePWM(stOutPin,stOut); 
}

void writePWM(int outServo,long msec){
       analogWrite(outServo,255);              //Pulse high for microvalue uS     
        delayMicroseconds(msec);
        analogWrite(outServo,0);                //Pulse low for 15mS
        delay(15);
    
}
