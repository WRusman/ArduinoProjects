//#include <TinyDebugSerial.h>
int thInPin=2, pwmPin=0, rxval=0;

//TinyDebugSerial mySerial=TinyDebugSerial();

void setup() {
  pinMode(thInPin,INPUT);
  pinMode(pwmPin,OUTPUT);
//  mySerial.begin(9600);
//  mySerial.println("SETUP Complete");
}

void loop() {
     rxval=constrain(pulseIn(thInPin, HIGH, 25000),140,240); 
//     mySerial.print(rxval);
//     mySerial.print(",");
//     mySerial.println(map(rxval,160,240,20,255));
  if (rxval>20) {
     analogWrite(pwmPin,map(rxval,140,240,0,255));
  }
  else
  {
    analogWrite(pwmPin,0);
  }
  }
