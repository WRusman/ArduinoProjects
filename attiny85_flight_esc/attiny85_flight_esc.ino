int thInPin=2, pwmPin=0, rxval=0;
// pin 2 = servo in, pin 0 = pwm out to FET

void setup() {
  pinMode(thInPin,INPUT);
  pinMode(pwmPin,OUTPUT);
}

void loop() {
     rxval=constrain(pulseIn(thInPin, HIGH, 25000),140,240);  // read input and constrain values between 140 and 240
  if (rxval>20) 
  {
     analogWrite(pwmPin,map(rxval,140,240,0,255)); // map output value 0-255 to input value 140-240
  }
  else 
  {
    analogWrite(pwmPin,0);
  }
}
