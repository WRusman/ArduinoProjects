int flasherPin1 =  8;      // the flasher for spiral 1 is connected to pin 8
int hornPin1 =  9;         // the horn for spiral 1 is connected to pin 9
int flasherPin2 =  10;     // the flasher for spiral 2 is connected to pin 10
int hornPin2 =  11;        // the (ac) bell for spiral 2 is connected to pin 11
int sensePin1 = 2;         // the loop for spiral 1 is connected to pin 2 (INT0)
int sensePin2 = 3;         // the loop for spiral 2 is connected to pin 3 (INT1)
int hornCounter = 0;       // counter for creating the ac power to the bell

unsigned long startMillis1 = 0;
unsigned long startMillis2 = 0;
unsigned long ledStartMillis = 0;
unsigned long currentMillis = 0;
int spiralTouch1 = 0;
int spiralTouch2 = 0;
long flasherOnTime = 2000;           // milliseconds the flasher will be on
long hornOnTime = 300;               // milliseconds the horn/bell will be on
volatile byte readState1 = HIGH;
volatile byte readState2 = HIGH;


void setup() {
  pinMode(flasherPin1, OUTPUT);
  pinMode(flasherPin2, OUTPUT);
  pinMode(hornPin1, OUTPUT);
  pinMode(hornPin2, OUTPUT);
  pinMode(sensePin1, INPUT_PULLUP);
  pinMode(sensePin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensePin1), touch1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(sensePin2), touch2, CHANGE);
  ledStartMillis = millis();
  delay (2000); // to avoid sounding the horns and flashing the lights during start-up
}

void loop()
{

  if (spiralTouch1 == 1) startMillis1 = millis(); // if the spiral is touched, set the start time for the sequence
  if (spiralTouch2 == 1) startMillis2 = millis();
  currentMillis = millis(); // get the current time

  if (currentMillis - startMillis1 < flasherOnTime) digitalWrite(flasherPin1,1); // if the current time minus the starting time is less than the on time, switch the output on 
  else digitalWrite(flasherPin1,0);                                              // else switch the output off

  if (currentMillis - startMillis2 < flasherOnTime) digitalWrite(flasherPin2,1);
  else digitalWrite(flasherPin2,0); 

  if (currentMillis - startMillis1 < hornOnTime) digitalWrite(hornPin1,1);
  else digitalWrite(hornPin1,0); 

  // routine to create 50Hz ac on ~8v for the bell when it should be on
  if (currentMillis - startMillis2 < hornOnTime)
    {
     if (hornCounter <= 200)
     {
      analogWrite(hornPin2,230);
      hornCounter++; 
     }
     else if ((hornCounter > 200) and (hornCounter <=400 ))
     {
      analogWrite(hornPin2,0);
      hornCounter++; 
     }
     else
     {
      hornCounter = 0;
     }
    }
  else analogWrite(hornPin2,0);

  // LED Flashing routine
  if (currentMillis - ledStartMillis <= 100) digitalWrite (LED_BUILTIN,HIGH);
  else if ((currentMillis - ledStartMillis > 100) and (currentMillis - ledStartMillis < 1000)) digitalWrite (LED_BUILTIN,LOW);
  else ledStartMillis = millis();
}


void touch1()                                 //interrupt service routine for the loop of spiral 1
{
  readState1 = digitalRead (sensePin1);       // the routine is triggered by a state change, here we read the current state
  if (readState1 == LOW) spiralTouch1 = 1;    // if the spral is touching, set spiraltouch to 1 
  else spiralTouch1 = 0;                      // if the spiral isn't touching anymore set it to 0
}
 
void touch2()
{
  readState2 = digitalRead (sensePin2);
  if (readState2 == LOW) 
  {
    spiralTouch2 = 1;
    hornCounter = 0;
  }
  else spiralTouch2 = 0;
}

