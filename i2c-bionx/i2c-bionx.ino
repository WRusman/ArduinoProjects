/* IC2 Bionx Console simulator. Duplicate some functions of the console 
  * Measure strain gauge and apply motor power in response
  * Report to the arduino serial port critical measurements
  */

/*
 * Copyright Daniel Cormode 2012. Free for personal use, permission required for use in a commercial product
 * SCL is connected to Arduino analog pin 5  
 * SDA to analog pin 4.
 */
 
#define averagedpoints 30  // The number of points used to compute the rolling average of the strain gauge

#include <Wire.h>
unsigned long lastsecond;
unsigned long lastreport;
int lasttwenty;
int lastten;
int powersetting;
int rotationspeed;
int straingauge;
int motorloading;
int errorcode;
int averagestrain[ averagedpoints ] ;
unsigned long summedstrain;
int strainpoint;
int poweroverride;
int keepbatteryawake;

void setup()
{
 int data;
 // Open serial port
 Wire.begin();
 Serial.begin(115200);
 
 // Zero array used for averaging
 for (int i = 0; i++ ;i < averagedpoints)
  {
    averagestrain[i] = 0;
  }
  summedstrain = 0;
  strainpoint = 0;
  
  // Three commands to make the bionx battery connect and close the power relay.
  Serial.print("   battery  register 32 =  ");   // relay keep alive
  // wake up battery
  data = ReadSingleByte(9,32);
  Serial.println(data);
  
  Serial.print("   bat 48 =  ");   // relay keep alive
  // battery is I2C address 9
  data = ReadSingleByte(9,48);
  Serial.print(data);
  
  Serial.print("   bat 49 =  ");   // relay keep alive
  data = ReadSingleByte(9,49);
  Serial.print(data);
 
 
 Serial.println("Waiting to motor to become active");
 // initalize motor
 while ( ReadSingleByte(8,32) < 1)
 {
   delay (100);
 }
 
  Serial.print("   battery  register 32 =  ");   // relay keep alive
  // wake up battery
  data = ReadSingleByte(9,32);
  Serial.println(data);
 
 // mimic three commands which console writes at bootup
 Serial.println("Initializing motor");
 WriteSingleByte(8,2,0);  // initialize strain gauge ?
 delay (10);
 WriteSingleByte(8,65,0);  // set motor to apply torque from a stopped position.  Setting this to 1 forces motor to be already spinning before it will apply power.
 delay (10);
 WriteSingleByte(8,66,1);  // set motor direction as forward
 delay(5);
 
 lastsecond =0;
 lasttwenty = 0;
 lastten = 0;
 lastreport = 0; 
 poweroverride = 0;
 keepbatteryawake = 1;
  
}

// Routine to read a single byte register from the I2C bus
int ReadSingleByte(int address, int reg)
{
 // reg = register to read
 // address = device address. Bionx I2C motor is 8
 int data = -1;
 int timeout = 10;

 Wire.beginTransmission(address);
 Wire.send(reg);
 if (Wire.endTransmission() == 0)
 {
 Wire.requestFrom(address,1);
 while (data == -1 & timeout > 0){  // sometimes the motor seems to try clock stretching.  Allow this for 10 ms.
   if (Wire.available() > 0)
   {
     data = Wire.receive();
   }
   timeout -=1;
   delay(1);
   }
 }
 
 return data; 
}

// routine to write a single byte register on the I2C bus
void WriteSingleByte(int address, int reg, int data)
{
 Wire.beginTransmission(address);
 Wire.send(reg);
 Wire.send(data);
 Wire.endTransmission(); 
}

void loop()
{  
  // read the strain gauge and update the power command 20 times per second.
  
  // calculate if it is time to communicate with the motor
  int twenty = (millis() % 1000) / 20;
  if (twenty != lasttwenty )
  {
    lasttwenty = twenty;
    // read strain gauge at register 8.
    straingauge = ReadSingleByte(8,21);
    if (straingauge < 0) straingauge = 0;  // check that the strain gauge value is valid
 
    // do the math to calculate the average straingague value over the last 1.5 seconds
    // The motor reports instaneous stain gauge values. As I pedal this value changes dramatically depending on pedal position
    // Without averaging the proportional response is herky-jerky
    
    // This is done by keeping a log of the last few measurements.
    
    averagestrain[strainpoint] = straingauge;  // replace the appropriate position in the array of historical points
    strainpoint++;     // update the index used to find the current place in the historical array
    if (strainpoint == averagedpoints)  
      strainpoint = 0;
    
    // sum the log and divide by twenty to   
    summedstrain = 0;
    for (int i = 0; i < averagedpoints;i++){
      summedstrain += averagestrain[i];
    }
    
    summedstrain = summedstrain / averagedpoints;
    
    // reduce the average strain by 1 as the motor seems to have some self strain when running.
    // The strain guage is mostly immune to the torsional force of the motor, but not completely
    // The strain guage does respond well to the lateral force of the chain pulling on the axle.
    summedstrain -= 1;
    if (summedstrain < 0) summedstrain = 0;
    
    // the power command is some multiple of the strain gauge (proportional power)
    powersetting = summedstrain * 1;

    // check if the user has entered a command previously at the serial port to run the motor faster
    if (poweroverride > powersetting) {
      powersetting = poweroverride;
    }
    
    // sanity check to keep from overdriving the motor
    // commanded power over 64 cause the motor to not operate
    if (powersetting > 64) {
      powersetting = 64;
    }
    
    // write the power command to the motor
    WriteSingleByte(8,9,powersetting);
    delay(1);
  }
  
  int ten = (millis() % 1000) / 10;
  if (ten != lastten )
  {
    lastten = ten;
    // rotation speed is register 17
    rotationspeed = ReadSingleByte(8,17);
    delay(1);
    // motor loading is register 20
    motorloading = ReadSingleByte(8,20);
    delay(1);
  }
  
  int second = millis() / 1000;
  if (second != lastsecond){
    lastsecond = second;
    errorcode = ReadSingleByte(8,18);
  }
  
  int report = millis() / 2500;   //report to user and keep alive battery pack
  if (report != lastreport){
    lastreport = report;  
    printreport();
  }
  
  delay(1);
 
}

void printreport()
{
  int data;
  int user;
  char useradd[4];
  char userdata[4];
  int useraddi;
  int userdatai;
  char userdev[4];
  int userdevi;
  int i;  

  
 // troublesome motor code
 
 Serial.print("    pow cmd =  ");  
 Serial.print(powersetting); 
 
 Serial.print("    mot 18 =  ");  
 Serial.print(errorcode);

  //motor related
  
  Serial.print("    mot rotation =  ");   // rotation speed
  Serial.print(rotationspeed);


  Serial.print("    mot torque =  ");   // motor loading
  Serial.print(motorloading);

  
  Serial.print("    inst strain gauge =  ");   // strain gauge
  Serial.print(straingauge);
  
 // Battery Related
 
 if (keepbatteryawake)
 
 {
   
  Serial.print("   bat 48 =  ");   // relay keep alive
  // battery is I2C address 9
  data = ReadSingleByte(9,48);
  Serial.print(data);
  
  Serial.print("   bat 49 =  ");   // relay keep alive
  data = ReadSingleByte(9,49);
  Serial.print(data);
  
 }
  
  Serial.print("   bat 50 =  ");   //  seems to important to console
  data = ReadSingleByte(9,50);
  Serial.print(data);
  
  Serial.print("   bat 51 =  ");   //  seems to important to console
  data = ReadSingleByte(9,51);
  Serial.print(data);
  
  Serial.print("   bat 96 =  ");   //  seems to important to console
  data = ReadSingleByte(9,96);
  Serial.print(data);
  
  Serial.print("   bat 97 =  ");   //  seems to important to console
  data = ReadSingleByte(9,97);
  Serial.print(data);
  
 

// read from the serial port to see if the user wants to issue a command

  if (Serial.available() > 0)
  
  {
    for ( i =0; i<3;i++){
    userdev[i] = Serial.read();
    }
    
    userdev[i] = '\0';
          
    user = Serial.read();
    
    for ( i =0; i<3;i++){
    useradd[i] = Serial.read();
    }
    
    useradd[i] = '\0';
    
    user = Serial.read();
    
   for (i =0; i<3;i++){
    userdata[i] = Serial.read();
    }
    
    userdata[i] = '\0';
   
   Serial.println("");
   Serial.print(useradd);
   Serial.print(" ");
   Serial.print(userdata);
   Serial.println(""); 
   
    userdevi = atoi(userdev);
    useraddi = atoi(useradd);
    
    userdatai = atoi(userdata);
   
   Serial.println("");
   Serial.print(userdevi);
   Serial.print(" ");
   Serial.print(useraddi);
   Serial.print(" ");
   Serial.print(userdatai);
   Serial.println("");
    
    if (useraddi > 0 & useraddi < 256 & userdatai > -1 & userdatai < 256) {
      WriteSingleByte(userdevi,useraddi,userdatai);
    }
    
    if (userdevi == 8 & useraddi == 9) {
      poweroverride = userdatai;
      Serial.print("Power override set to");
      Serial.println(poweroverride);
    }
    
    
    
    if ((userdevi == 9 & useraddi == 33) & userdatai == 0) {
      keepbatteryawake = 0;
      Serial.print("Letting battery relay stay closed");         
    }
    
    if ((userdevi == 9 & useraddi == 33) & userdatai == 1) {
      keepbatteryawake = 1;
      Serial.print("Keeping battery relay open");         
    }
  }    
  
  Serial.println("");
}
