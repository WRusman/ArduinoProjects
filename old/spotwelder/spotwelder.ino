 /*
  * 
 * The circuit:
 * LCD (4) RS pin to digital pin 12
 * LCD (6) Enable pin to digital pin 11
 * LCD (11) D4 pin to digital pin 5
 * LCD (12) D5 pin to digital pin 4
 * LCD (13) D6 pin to digital pin 3
 * LCD (14) D7 pin to digital pin 2
 * LCD R/W pin to ground
 
 Solid State Relay to digital pin 8
 Trigger switch to digital pin 13
 menu button up to digital pin 6
 menu button down to digital pin 7
*/

#include <LiquidCrystal.h>
#include <EEPROM.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2, trig = 13, but_up = 6, but_down = 7, relay = 8;
int timerUp=0, timerDown=0, timer, value, weld;
char buff[10];

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  pinMode(trig,INPUT);
  pinMode(relay,OUTPUT);
  pinMode(but_up,INPUT);
  pinMode(but_down,INPUT);
  EEPROM.get(0, value);
  if ((value < 50) or (value > 1000)){
    value = 500;
    EEPROM.put(0, value);
  }
  timer = value;
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("MaakPuntlas v0.1");
  lcd.setCursor(0,1);
  lcd.print("Puls (ms):");
  updateTimer();
}

void loop() {
readMenuButtons();
readTrigger();
}


void readMenuButtons(){
  timerUp = digitalRead(but_up);
  timerDown = digitalRead(but_down);
 
  if (timerUp){
    if (timer < 1000){
      timer = timer+10;
      updateTimer();
      delay(100);
    }
  }
    
  if (timerDown){
    if (timer > 50){
      timer = timer-10;
      updateTimer();
      delay(100);
    }
  }
  
}

void readTrigger(){
    weld = digitalRead(trig);
    if (weld){
      digitalWrite(relay,1);
      delay(timer);
      digitalWrite(relay,0);  
      delay(1000);
  }
}

void updateTimer(){
      EEPROM.put(0, timer);
      lcd.setCursor(12,1);
      sprintf(buff, "%4d", timer);
      lcd.print(buff); 
}
