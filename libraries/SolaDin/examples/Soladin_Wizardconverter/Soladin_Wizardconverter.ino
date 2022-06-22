/* Use a Arduino as converter between Soladin
and Soladin Wizard.
The arduino comport will not show up in the Wizard dropbox.
instead you have to fill in by hand "COM6" in my case
Make sure arduino's ide Serial monitor is closed before.
*/



#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);

void setup()  
{
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
}

void loop() // run over and over
{
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());
}
