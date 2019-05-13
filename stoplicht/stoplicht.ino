int voetgangerRood=9;
int voetgangerGroen=10;
int autoRood=4;
int autoOranje=3;
int autoGroen=2;

int zwaaiLicht1=6;
int zwaaiLicht2=7;

int knopje1=11;
int knopje2=12;

void setup() {

pinMode(voetgangerRood,OUTPUT);
pinMode(voetgangerGroen,OUTPUT);
pinMode(autoRood,OUTPUT);
pinMode(autoOranje,OUTPUT);
pinMode(autoGroen,OUTPUT);
pinMode(knopje1,INPUT_PULLUP);
pinMode(knopje2,INPUT_PULLUP);
pinMode(zwaaiLicht1,OUTPUT);
pinMode(zwaaiLicht2,OUTPUT);
digitalWrite(voetgangerRood,HIGH);
digitalWrite(voetgangerGroen,LOW);
digitalWrite(autoRood,LOW);
digitalWrite(autoOranje,LOW);
digitalWrite(autoGroen,HIGH);
digitalWrite(zwaaiLicht1,LOW);
digitalWrite(zwaaiLicht2,LOW);
}

void loop() {
  int buttonPressed=pulseIn(knopje1,LOW);
  if (buttonPressed > 10){
    voetgangersMogen();
  }
  int doorRood=pulseIn(knopje2,LOW);
  if (doorRood > 10){
    politie();
  }

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
  }

void politie(){
  for (int x=0; x<=10; x++){
    for (int y=0; y<=3; y++){
        digitalWrite(zwaaiLicht1,HIGH);
        delay(50);
        digitalWrite(zwaaiLicht1,LOW);
        delay(50);
    }
    for (int z=0; z<=3; z++){
        digitalWrite(zwaaiLicht2,HIGH);
        delay(50);
        digitalWrite(zwaaiLicht2,LOW);
        delay(50);
    }
  }    
}
  
