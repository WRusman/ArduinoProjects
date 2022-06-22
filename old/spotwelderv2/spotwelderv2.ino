 /*
  * 
 * The circuit:
 * LCD (4) RS pin to digital pin 12
 * LCD (6) Enable pin to digital pin 11
 * LCD (11) D4 pin to digital pin 10
 * LCD (12) D5 pin to digital pin 9
 * LCD (13) D6 pin to digital pin 4
 * LCD (14) D7 pin to digital pin 5
 * LCD R/W pin to ground
 
 Solid State Relay to digital pin 8
 Trigger switch to digital pin 7
 rotary encoder  button to digital pin 6
 rotary encoder  clk to digital pin 3
 rotary encoder  data to digital pin 2
 
*/

#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Encoder.h>
#define ENCODER_OPTIMIZE_INTERRUPTS

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 4, d7 = 5, trig = 7, relay = 8, statusLed = 13, rBut = 6, rClk = 3, rData = 2 ;
int triggerButton, triggerState, prevTriggerState, encBuffer ,menuButton, menuState, prevMenuState, timer, pulses, eeAddress = 0;
char buff[10];
long rPosition  = -999, rNewPosition;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Encoder knob(rClk, rData);

void setup() {
  pinMode(rBut,INPUT_PULLUP);
  pinMode(trig,INPUT);
  pinMode(relay,OUTPUT);
  pinMode(statusLed,OUTPUT);
  EEPROM.get(eeAddress, timer);
  if ((timer < 50) or (timer > 1000)){
    timer = 500;
    EEPROM.put(eeAddress, timer);
  }
  eeAddress += sizeof(int);
  EEPROM.get(eeAddress, pulses);
  if ((pulses < 1) or (pulses > 5)){
    pulses = 3;
    EEPROM.put(eeAddress, pulses);
  }
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("MaakPuntlas v0.3");
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print("Pulselength:    ");
  lcd.setCursor(12,0);
  sprintf(buff, "%4d", timer);
  lcd.print(buff);
  lcd.setCursor(0,1);
  lcd.print("Nr of pulses:   ");
  lcd.setCursor(15,1);
  lcd.print(pulses); 
  triggerButton = 0;
  triggerState = 0;
  menuButton = 0;
  menuState = 1;
}

void loop() {
readMenuButton();
readTrigger();
weld();
setupMenu();
}

void readMenuButton(){
  menuState = digitalRead(rBut);
  if ((menuState != prevMenuState) and (menuState == 0 )) { menuButton = 1;}
  if ((menuState != prevMenuState) and (menuState == 1 )) { menuButton = 0;}
  if (menuState != prevMenuState) {delay(200);} 
  prevMenuState = menuState;
}

void readTrigger(){
  triggerState = digitalRead(trig);
  if ((triggerState != prevTriggerState) and (triggerState == 1 )) { triggerButton = 1;}
  if ((triggerState != prevTriggerState) and (triggerState == 0 )) { triggerButton = 0;}
  if (triggerState != prevTriggerState) {delay(200);} 
  prevTriggerState = triggerState;
}

void weld() {
  if (triggerButton == 1){
    for (int x=1; x <= pulses;x++){
      digitalWrite(relay,HIGH);
      digitalWrite(statusLed,HIGH);
      delay(timer);
      digitalWrite(relay,LOW);
      digitalWrite(statusLed,LOW);
      delay(timer);
    }
    delay(500);
  }
}

void setupMenu(){
  if (menuButton == 1){
    menuButton = 0;
    rPosition = knob.read();
    encBuffer = 4;
    
    while (menuButton == 0) {
      lcd.setCursor(12,0);
      lcd.blink();
      rNewPosition = knob.read();
      if (rNewPosition != rPosition) {
        if (((rNewPosition > rPosition) and (timer == 1000)) or ((rNewPosition < rPosition) and (timer == 50))) {
          for (int y=1;y <4; y++){
            lcd.noDisplay(); 
            delay(200);
            lcd.display();
            delay(200);
          }
        knob.write(rPosition); 
        }
        else
        {
          if (rNewPosition > (rPosition)){
            encBuffer++;
            rPosition = rNewPosition; 
          }
          if (rNewPosition < (rPosition)){
            encBuffer--;
            rPosition = rNewPosition; 
          }
          if (encBuffer == 8){timer++;encBuffer=4;}
          if (encBuffer == 0){timer--;encBuffer=4;}
          lcd.setCursor(12,0);
          sprintf(buff, "%4d", timer);
          lcd.print(buff);
          rPosition = rNewPosition; 
        }
      }
      readMenuButton();
    }
    
    menuButton = 0;
    rPosition = knob.read();
    encBuffer = 4;
        
    while (menuButton == 0) {
    lcd.setCursor(15,1);
    lcd.blink();
      rNewPosition = knob.read();
      if (rNewPosition != rPosition) {
        if (((rNewPosition > rPosition) and (pulses == 5)) or ((rNewPosition < rPosition) and (pulses == 1))) {
          for (int y=1;y <4; y++){
            lcd.noDisplay(); 
            delay(200);
            lcd.display();
            delay(200);
          }
        knob.write(rPosition); 
        }
        else
        {
          if (rNewPosition > (rPosition)){
            encBuffer++;
            rPosition = rNewPosition; 
          }
          if (rNewPosition < (rPosition)){
            encBuffer--;
            rPosition = rNewPosition; 
          }
          if (encBuffer == 8){pulses++;encBuffer=4;}
          if (encBuffer == 0){pulses--;encBuffer=4;}
          lcd.setCursor(15,1);
          lcd.print(pulses);
          rPosition = rNewPosition; 
        }
      }
      readMenuButton();
    }
    lcd.noBlink();
    eeAddress = 0;        
    EEPROM.put(eeAddress, timer);
    eeAddress += sizeof(int);
    EEPROM.put(eeAddress, pulses);
    delay(1000);
  }
}
