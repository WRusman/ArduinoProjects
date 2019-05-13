/*
# Uses https://github.com/marcoschwartz/LiquidCrystal_I2C
# and https://github.com/miguelbalboa/rfid
#Arduino nano
# D2  = Led K
# D9  = RC522 RESET
# D10 = RC522 SDA  
# D11 = RC522 MOSI 
# D12 = RC522 MISO 
# D13 = RC522 SCK  
# A0  = BUZZER
# A4  = LCD SDA
# A5  = LCD SCL
# 
*/

#include <EEPROM.h>       // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>          // RC522 Module uses SPI protocol
#include <MFRC522.h>      // Library for Mifare RC522 Devices
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

boolean match = false;    // initialize card match to false
uint8_t successRead;      // Variable integer to keep if we have Successful Read from Reader
byte storedCard[4];       // Stores an ID read from EEPROM
byte readCard[4];         // Stores scanned ID read from RFID Module
const int buzzer = A0;    // buzzer to arduino pin analog 0
const int ledpin = 2;     // LED to arduino pin 2

LiquidCrystal_I2C lcd(0x38,20,4);  // set the LCD address to 0x38
MFRC522 mfrc522(10,9);

void setup() {
  Serial.begin(9600);     // Initialize serial communications with PC
  SPI.begin();            // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();     // Initialize MFRC522 Hardware
  //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  pinMode(ledpin, OUTPUT);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Welkom bij Arriva");
  lcd.setCursor(5,2);
  lcd.print("Groningen");
  lcd.setCursor(1,3);
  lcd.print("In-/Uitchecken AUB");
  
  digitalWrite(ledpin,HIGH);
  
  Serial.println(F("checkin checkout v0.1"));   // For debugging purposes
  ShowReaderDetails();    // Show details of PCD - MFRC522 Card Reader details

      Serial.println(F("Starting Wiping EEPROM"));
      for (uint16_t x = 0; x < EEPROM.length(); x = x + 1) {    //Loop end of EEPROM address
        if (EEPROM.read(x) == 0) {              //If EEPROM address 0
        }
        else {
          EEPROM.write(x, 0);       // if not write 0 to clear, it takes 3.3mS
        }
      }
  Serial.println(F("EEPROM Successfully Wiped"));
  Serial.println(F("-------------------"));
  Serial.println(F("Everything is ready"));
  Serial.println(F("Waiting PICCs to be scanned"));
}


void loop () {
  do {
    successRead = getID();      // sets successRead to 1 when we get read from reader otherwise 0
  }
  while (!successRead);         //the program will not go further while you are not getting a successful read
      if ( findID(readCard) ) { // If scanned card is known delete it
        Serial.println(F("I know this PICC, removing..."));
        deleteID(readCard);
        Serial.println("-----------------------------");
        Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      }
      else {                    // If scanned card is not known add it
        Serial.println(F("I do not know this PICC, adding..."));
        writeID(readCard);
        Serial.println(F("-----------------------------"));
        Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      }
}

uint8_t getID() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  Serial.println(F("Scanned PICC's UID:"));
  for ( uint8_t i = 0; i < 4; i++) {  
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA();       // Stop reading
  return 1;
}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown),probably a chinese clone?"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    Serial.println(F("SYSTEM HALTED: Check connections."));
    // Visualize system is halted
    while (true);               // do not go further
  }
}

void readID( uint8_t number ) {
  uint8_t start = (number * 4 ) + 2;    // Figure out starting position
  for ( uint8_t i = 0; i < 4; i++ ) {   // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
}

void writeID( byte a[] ) {
  if ( !findID( a ) ) {               // Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t start = ( num * 4 ) + 6;  // Figure out where the next slot starts
    num++;                            // Increment the counter by one
    EEPROM.write( 0, num );           // Write the new count to the counter
    for ( uint8_t j = 0; j < 4; j++ ) {   // Loop 4 times
      EEPROM.write( start + j, a[j] );    // Write the array values to EEPROM in the right position
    }
    successWrite();
    Serial.println(F("Succesfully added ID record to EEPROM"));
  }
  else {
    failedWrite();
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
}

void deleteID( byte a[] ) {
  if ( !findID( a ) ) {               // Before we delete from the EEPROM, check to see if we have this card!
    failedWrite();                    // If not
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
  else {
    uint8_t num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t slot;                     // Figure out the slot number of the card
    uint8_t start;                    // = ( num * 4 ) + 6; // Figure out where the next slot starts
    uint8_t looping;                  // The number of times the loop repeats
    uint8_t j;
    uint8_t count = EEPROM.read(0);   // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a );           // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;                            // Decrement the counter by one
    EEPROM.write( 0, num );           // Write the new count to the counter
    for ( j = 0; j < looping; j++ ) { // Loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
    }
    for ( uint8_t k = 0; k < 4; k++ ) {         // Shifting loop
      EEPROM.write( start + j + k, 0);
    }
    successDelete();
    Serial.println(F("Succesfully removed ID record from EEPROM"));
  }
}

boolean checkTwo ( byte a[], byte b[] ) {
  if ( a[0] != 0 )                    // Make sure there is something in the array first
    match = true;                     // Assume they match at first
  for ( uint8_t k = 0; k < 4; k++ ) { // Loop 4 times
    if ( a[k] != b[k] )               // IF a != b then set match = false, one fails, all fail
      match = false;
  }
  if ( match ) {                      // Check to see if if match is still true
    return true;                      // Return true
  }
  else  {
    return false;                     // Return false
  }
}

uint8_t findIDSLOT( byte find[] ) {
  uint8_t count = EEPROM.read(0);     // Read the first Byte of EEPROM that
  for ( uint8_t i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);                        // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
                                      // is the same as the find[] ID card passed
      return i;                       // The slot number of the card
      break;                          // Stop looking we found it
    }
  }
}

boolean findID( byte find[] ) {
  uint8_t count = EEPROM.read(0);     // Read the first Byte of EEPROM that
  for ( uint8_t i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);                        // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;                          // Stop looking we found it
    }
    else {                            // If not, return false
    }
  }
  return false;
}

void successWrite() {
  digitalWrite(ledpin,LOW);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Welkom bij Arriva");
  lcd.setCursor(4,2);
  lcd.print("Inchecken ok");
  tone(buzzer, 3000); // Send 1KHz sound signal...
  delay(80);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(100);        // ...for 1sec
  tone(buzzer, 3000); // Send 1KHz sound signal...
  delay(80);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(2000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Welkom bij Arriva");
  lcd.setCursor(5,2);
  lcd.print("Groningen");
  lcd.setCursor(1,3);
  lcd.print("In-/Uitchecken AUB");
  digitalWrite(ledpin,HIGH);
}

void failedWrite() {
  lcd.setCursor(0, 3);
  lcd.print("Schrijffout");
  tone(buzzer, 500); // Send 1KHz sound signal...
  delay(3000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
}

void successDelete() {
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Welkom bij Arriva");
  lcd.setCursor(4,2);
  lcd.print("Uitchecken ok");
  digitalWrite(ledpin,LOW);
  tone(buzzer, 3000); // Send 1KHz sound signal...
  delay(80);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(2000);
  digitalWrite(ledpin,HIGH);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Welkom bij Arriva");
  lcd.setCursor(5,2);
  lcd.print("Groningen");
  lcd.setCursor(1,3);
  lcd.print("In-/Uitchecken AUB");
}


