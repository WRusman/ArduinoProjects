int autoRood=3;
int autoOranje=4;
int autoGroen=5;
int voetgangerRood=6;
int voetgangerGroen=7;
int knopje1=8;
int knopje2 = 2;                    //  <-- knopje 2 verhuisd naar pin 2, daarop kan een interrupt worden gelezen (INT0)
int zwaaiLicht1=10;                 //  <-- twee extra LED's voor de zwaailichten
int zwaaiLicht2=11;
boolean autoMoetStoppen=false;      //  <-- een nieuwe variabele om de status van het stoplicht in op te slaan.
boolean doorRood=false;             //  <-- een nieuwe variabele om de status van het door-rood-rijden in op te slaan.
volatile byte readState = LOW;      //  <-- een nieuwe variabele om de status van het tweede knopje in op te slaan

void setup() {
pinMode(voetgangerRood,OUTPUT);
pinMode(voetgangerGroen,OUTPUT);
pinMode(autoRood,OUTPUT);
pinMode(autoOranje,OUTPUT);
pinMode(autoGroen,OUTPUT);
pinMode(knopje1,INPUT);
pinMode(knopje2,INPUT);
pinMode(zwaaiLicht1,OUTPUT);
pinMode(zwaaiLicht2,OUTPUT);
digitalWrite(voetgangerRood,HIGH);
digitalWrite(voetgangerGroen,LOW);
digitalWrite(autoRood,LOW);
digitalWrite(autoOranje,LOW);
digitalWrite(autoGroen,HIGH);
digitalWrite(zwaaiLicht1,LOW);       // <-- standaard staan de zwaailichten uit
digitalWrite(zwaaiLicht2,LOW);
attachInterrupt(digitalPinToInterrupt(knopje2), politie, CHANGE);  //  <-- maak de interruptroutine "politie()" vast aan pin 2.
                                                                   //  een interrupt gaat dwars door alle subroutines heen
}

void loop() {
if (spiralTouch1 == 1) startMillis1 = millis();

}

void voetgangersMogen(){
  delay(1000);
  digitalWrite(autoGroen,LOW);
  digitalWrite(autoOranje,HIGH);
  delay(3000);
  digitalWrite(autoOranje,LOW);
  digitalWrite(autoRood,HIGH);
  delay(1000); 
  digitalWrite(voetgangerRood,LOW);
  digitalWrite(voetgangerGroen,HIGH);
  autoMoetStoppen = true;
  delay(9000);
  for (int x=0; x<=3; x++){
    digitalWrite(voetgangerGroen,LOW);
    delay(500);
    digitalWrite(voetgangerGroen,HIGH);
    delay(500);
    }
  digitalWrite(voetgangerRood,HIGH);
  digitalWrite(voetgangerGroen,LOW);
  delay(1000);
  digitalWrite(autoRood,LOW);
  digitalWrite(autoGroen,HIGH);
  autoMoetStoppen = false;
  }

void politie(){
  readState = digitalRead (knopje2);     
  if ((readState == HIGH ) and (autoMoetStoppen == true)) {
  doorRood = 1; 
  }      
  else
  doorRood = 0;
}
  
