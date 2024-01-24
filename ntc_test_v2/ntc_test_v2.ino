#include <PID_v1.h>
#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define SENSORTOP_PIN             A6
#define SENSORINSIDE_PIN          A7

#define REFERENCE_RESISTANCE   10000
#define NOMINAL_RESISTANCE     10000
#define NOMINAL_TEMPERATURE    25
#define B_VALUE                4300

//Define Variables we'll be connecting to
double Setpoint, Input, Output;
//Define the aggressive and conservative Tuning Parameters

double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);
Thermistor* thermistorTop;
Thermistor* thermistorInside;
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  Serial.begin(9600);
    Setpoint = 50;

  thermistorTop = new NTC_Thermistor(
    SENSORTOP_PIN,
    REFERENCE_RESISTANCE,
    NOMINAL_RESISTANCE,
    NOMINAL_TEMPERATURE,
    B_VALUE
  );
  thermistorInside = new NTC_Thermistor(
    SENSORINSIDE_PIN,
    REFERENCE_RESISTANCE,
    NOMINAL_RESISTANCE,
    NOMINAL_TEMPERATURE,
    B_VALUE
  );
  myPID.SetMode(AUTOMATIC);
  
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(5,0);
  lcd.print("BiBrew");
  delay(1000);
  lcd.clear();
}

void loop()
{
    // Reads temperature
  const double celsiusTop = thermistorTop->readCelsius();
  const double celsiusInside = thermistorInside->readCelsius();
  // Output of information
  Serial.print("Top Temperature: ");
  Serial.print(celsiusTop);
  Serial.println(" °C, ");
  Serial.print("Inside Temperature: ");
  Serial.print(celsiusInside);
  Serial.println(" °C, ");

  Input = celsiusTop;
  
  double gap = abs(Setpoint-Input); //distance away from setpoint
  if (gap < 10)
  {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  }
  else
  {
     //we're far from setpoint, use aggressive tuning parameters
     myPID.SetTunings(aggKp, aggKi, aggKd);
  }
  
  myPID.Compute();
  analogWrite(3,255-Output);
  Serial.print("Output Power: ");
  Serial.println(255-Output);
  Serial.println("");
  

  lcd.setCursor(0,0);
  lcd.print("Top: ");
  lcd.print(celsiusTop);
  lcd.print(" \337C");
  
  lcd.setCursor(0,1);
  lcd.print("Inside: ");
  lcd.print(celsiusInside);
  lcd.print(" \337C");
  delay(1000);
}
