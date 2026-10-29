int latchPin = 9;  // D9 on the arduino Nano
int dataPin = 11;  // D11 on the arduino Nano
int clockPin = 13; // D13 on the arduino Nano
int resetPin = 4;  // D4 on the arduino Nano
int startPin = 5;  // D5 on the arduino Nano
int stopPin = 6;   // D6 on the arduino Nano
int OEPin = 7;     // D7 on the arduino Nano
int clearPin = 8;  // D8 on the arduino Nano
int startCounter, stopCounter, resetCounter;
int remTime, countdownTime=30;
bool running, paused;


//     
//  -8-
// 3|  |7
//  -2-
// 4|  |6
//  -5-
// bit 1 = not used

//0 = 00111111 = 63
//1 = 00000110 = 6
//2 = 01011011 = 91
//3 = 01001111 = 79
//4 = 01100110 = 102
//5 = 01101101 = 109
//6 = 01111101 = 125
//7 = 00000111 = 7
//8 = 01111111 = 127
//9 = 01101111 = 111

byte segChar[] = { 63, 6, 91, 79, 102, 109, 125, 7, 127, 111, 0};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(OEPin, OUTPUT);  
  pinMode(clearPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(startPin, INPUT_PULLUP);
  pinMode(stopPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  digitalWrite(latchPin, LOW);
  digitalWrite(clearPin, HIGH);
  Serial.begin(115200);
  remTime=countdownTime;
  
  showDisplay();
}

void loop() {
  if (debounceButton(startPin)) {
        running = true;
        
    }
    if (debounceButton(stopPin)) {
        running = false;
        paused = true;
    }
    if ((!running) && debounceButton(resetPin)) {
        paused = false;
        remTime = countdownTime;
        showDisplay();
    }
    
    if (running && remTime > 0) {
        delay(1000); // 1-second delay
        remTime--;
        showDisplay();
    }

    
    if (!running && !paused){
      readTimeSwitch();
      remTime = countdownTime;
      showDisplay();  
      delay(50);  
    }
}


// Debounce function
bool debounceButton(int pin) {
    static uint32_t lastPressTime = 0;
    uint32_t currentTime = millis();
    if (digitalRead(pin) == LOW) {
        if (currentTime - lastPressTime > 50) { // 50ms debounce time
            lastPressTime = currentTime;
            return true;
        }
    }
    return false;
}

void readTimeSwitch(){
    countdownTime=30;
    if (!digitalRead(A0)) countdownTime=60;
    if (!digitalRead(A1)) countdownTime=90;
    if (!digitalRead(A2)) countdownTime=120;
    if (!digitalRead(A3)) countdownTime=150;   
    if (!digitalRead(A4)) countdownTime=180;  
}

void showDisplay(){
  int remMinutes=((remTime/60) %10);
  int remSeconds=(remTime-(remMinutes*60));
  digitalWrite(OEPin, HIGH);  
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[remMinutes]);          // digit 1
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[remSeconds /10 %10]);  // digit 2
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[remSeconds %10]);      // digit 3
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
  digitalWrite(OEPin, LOW);
}
