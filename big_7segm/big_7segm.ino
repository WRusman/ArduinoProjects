#include <DS1307RTC.h>
#include <Timezone.h>

// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone myTZ(CEST, CET);

TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev

int strobePin = 3;
int dataPin = 4;
int clockPin = 5;

byte segChar[] = { 246, 48, 110, 122, 184, 218, 222, 112, 254, 250, 127, 0};
int hour1,hour2,minute1,minute2,nowminutes,nowhours;
bool colon = true;

void setup() {
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  colon=true;
  Serial.begin(57600);

    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus()!= timeSet)
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");
  
}

void loop() {
    time_t utc = now();
    time_t local = myTZ.toLocal(utc, &tcr);

    nowminutes=now.minute();
    nowhours=now.hour();

    if (nowminutes < 10) {
      minute2=nowminutes;      
      minute1=0;
    }
    else {
      minute2=nowminutes % 10;      
      minute1=(nowminutes /10) % 10;
    }
    
    if (nowhours < 10) {
      hour2=nowhours;      
      hour1=0;
    }
    else {
      hour2=nowhours % 10;      
      hour1=(nowhours/10) % 10;
    }

    Serial.print(hour1);
    Serial.print(hour2);
    Serial.print(":");
    Serial.print(minute1);
    Serial.println(minute2);

    digitalWrite(strobePin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[minute2]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[minute1]);  
    if (colon==true) {shiftOut(dataPin, clockPin, MSBFIRST, segChar[10]);}else{shiftOut(dataPin, clockPin, MSBFIRST, segChar[11]);}  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[hour2]);  
    shiftOut(dataPin, clockPin, MSBFIRST, segChar[hour1]);  
    digitalWrite(strobePin, HIGH);

    delay(1000);
    
    if (colon==true){
      colon=false;
    }
    else{
      colon=true;
    }
   
}
