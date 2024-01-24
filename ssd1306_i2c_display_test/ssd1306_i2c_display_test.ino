#include <Wire.h>                     // voor I2C-communicatie
#include <Adafruit_GFX.h>             // Adafruit Graphics Library
#include <Adafruit_SSD1306.h>         // Adafruit OLED-displaydriver

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 128
#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialiseer OLED-display met het juiste I2C-adres
  display.clearDisplay(); // wis het display
  display.setTextSize(2); // kies tekstgrootte
  display.setTextColor(WHITE); // kies tekstkleur
}

void loop() {
  display.clearDisplay(); // wis het display
  display.setCursor(0,0); // zet cursor aan het begin
  display.println("Maakplek"); // voeg tekst toe aan display
  display.display(); // toon de tekst op het display}
delay(1000);
  display.clearDisplay(); // wis het display
  display.setCursor(0,0); // zet cursor aan het begin
  display.println("Random"); // voeg tekst toe aan display
  display.setCursor(0,15); // zet cursor aan het begin
  display.println("Object"); // voeg tekst toe aan display
  display.display(); // toon de tekst op het display}
 delay(1000);
  display.clearDisplay(); // wis het display
  display.setCursor(0,0); // zet cursor aan het begin
  display.println("Build"); // voeg tekst toe aan display
  display.setCursor(0,15); // zet cursor aan het begin
  display.println("Challenge"); // voeg tekst toe aan display
  display.display(); // toon de tekst op het display}
 delay(3000);
}
