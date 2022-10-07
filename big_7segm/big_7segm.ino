#include <DS1307RTC.h>
#include <Timezone.h>

// RTC has to be set to UTC, the program will adjust the RTC time according to the TimeChangeRule

  TimeChangeRule *tcr;
  TimeChangeRule nlDST = {"DST", Last, Sun, Mar, 1, +120};  //UTC + 2 hours
  TimeChangeRule nlSTD = {"STD", Last, Sun, Oct, 1, +60};   //UTC + 1 hour
  Timezone myTZ(nlDST, nlSTD);
  
int strobePin = 3;
int dataPin = 4;
int clockPin = 5;
// SDA = A4
// SCL = A5

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
    Serial.println();
    printDateTime(local, tcr -> abbrev);
    delay(1000);
}


// format and print a time_t value, with a time zone appended.
void printDateTime(time_t t, const char *tz)
{
    char buf[32];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
        hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);

    nowminutes=minute(t);
    nowhours=hour(t);

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
