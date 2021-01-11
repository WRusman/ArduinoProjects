void setup() {  
  pinMode(0, OUTPUT); // attiny pin 5
  pinMode(1, OUTPUT); // attiny pin 6
  pinMode(2, OUTPUT); // attiny pin 7
  pinMode(3, OUTPUT); // attiny pin 2
  pinMode(A2, INPUT); // attiny pin 3
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
}

void loop() {
  // half-step drive using input from potmeter as speed control
  int delay1=map(analogRead(A2),0,255,20,80);
  digitalWrite(0, HIGH); //1001
  delay(delay1);                    
  digitalWrite(3, LOW);  //1000
  delay(delay1);                    
  digitalWrite(2, HIGH); //1010
  delay(delay1);                    
  digitalWrite(0, LOW);  //0010
  delay(delay1);                    
  digitalWrite(1, HIGH); //0110
  delay(delay1);                    
  digitalWrite(2, LOW);  //0100
  delay(delay1);                    
  digitalWrite(3, HIGH); //0101
  delay(delay1);                    
  digitalWrite(1, LOW);  //0001
  delay(delay1);                    
}
