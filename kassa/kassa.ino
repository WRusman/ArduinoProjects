void setup() {
  // put your setup code here, to run once:
pinMode(7, OUTPUT);
pinMode(8,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
bool buttonPress = digitalRead(8); 
if (buttonPress == 0){
tone(7,2000,100);
delay(200);
}
}
