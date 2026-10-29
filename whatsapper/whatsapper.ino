#include "Keyboard.h"

#define buttonPin 3    // the number of the pushbutton pin
#define enablePin 4    // the number of the enable switch

int buttonState = LOW;       // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int maakplekState = LOW;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(enablePin, INPUT_PULLUP);
  
Serial.begin(9600);
Serial.println("start");
Serial1.begin(9600); //This is the UART, pipes to sensors attached to board
Serial1.println("start");
Keyboard.begin();
}


void sendOpen(){
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);       
    
    Keyboard.press (KEY_LEFT_SHIFT); Keyboard.print("whatsapp");Keyboard.releaseAll(); delay(1000);
    Keyboard.press (KEY_RETURN); Keyboard.releaseAll(); delay(5000); 
      
    Keyboard.print("maakplek"); delay(2000);
    Keyboard.press (KEY_DOWN_ARROW); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_DOWN_ARROW); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_RETURN); Keyboard.releaseAll(); delay(5000);   
   
    Keyboard.print("Maakplek is geopend!");delay(1000);
    Keyboard.press (KEY_RIGHT_ARROW); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_RIGHT_ARROW); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_RETURN); Keyboard.releaseAll(); delay(5000);   
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);       
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);       
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);       
 }

void sendClosed(){
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);       
    
    Keyboard.press (KEY_LEFT_SHIFT); Keyboard.print("whatsapp");Keyboard.releaseAll(); delay(1000);
    Keyboard.press (KEY_RETURN); Keyboard.releaseAll(); delay(5000); 
      
    Keyboard.print("maakplek"); delay(2000);
    Keyboard.press (KEY_DOWN_ARROW); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_DOWN_ARROW); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_RETURN); Keyboard.releaseAll(); delay(5000);   
   
    Keyboard.print("Maakplek is gesloten!");delay(1000);
    Keyboard.press (KEY_RIGHT_ARROW); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_RIGHT_ARROW); Keyboard.releaseAll(); delay(1000);   
    Keyboard.press (KEY_RETURN); Keyboard.releaseAll(); delay(5000);   
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);       
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);       
    Keyboard.press (KEY_ESC); Keyboard.releaseAll(); delay(1000);       
 }


void loop() {
    int enableState = digitalRead(enablePin);
    if (enableState == 0) {
     Serial.println("enabled");
      int buttonState = digitalRead(buttonPin);
      if (buttonState == 1){
        if (maakplekState != buttonState){
        Serial.println("Maakplek Open");
        sendOpen();  
        maakplekState = HIGH;  
        }
      }
      else
      {
        if (maakplekState != buttonState){
        Serial.println("Maakplek Dicht");
        sendClosed();  
        maakplekState = LOW;  
        }
      }
    }
    else {
   Serial.println("disabled");
    }  
}
