#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
}
void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //delay(1000);
}
