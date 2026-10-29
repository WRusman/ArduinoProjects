#include <Soladin.h>
#include <SoftwareSerial.h>

SoftwareSerial solcom(2, 3);                            // serial to conect to soladin  
Soladin sol ;						// copy of soladin class
boolean connect = 0 ;					// if soladin respons

void setup()
{
  Serial.begin(9600);
  Serial.print("Setup...");
  solcom.begin(9600);
  sol.begin(&solcom);
  Serial.println("...done");
  Serial.println("------menu------------");
  Serial.println("f - Firmware");
  Serial.println("r - Read Max power");  
  Serial.println("o - Reset Max power");  
  Serial.println("h - Read hystory data");
  Serial.println("d - Read device status");
  Serial.println("x - Write RXbuffer");
  Serial.println();
}

void loop() {                   
  if (!connect) {      					// Try to connect
    Serial.print("Cmd: Probe");
    for (int i=0 ; i < 4 ; i++) {
      if (sol.query(PRB)) {				// Try connecting to slave
        connect = true; 
        Serial.println("...Connected");
        break; 
      } 
      Serial.print(".");
      delay(1000);
    }
  }
  if (Serial.available() > 0) {				// read serial comands
    char incomingByte = Serial.read();   
    switch (incomingByte) {
    case 'f':       					// read firmware  
      doFW();
      break;  
    case 'r':       					// read max power
      doMP();    
      break;      
    case 'd':      					// read device status    
      doDS();
      break;
    case 'o':      					// reset max power   
      doRMP();
      break;      
    case 'h':      					// read hystorical data   
      doHD();
      break;       
    case 'x':      					// print buffer    
      doX();
      break;   
    }
  }
}


void doFW(){
  if (connect)  {      					// already connected
    SPrintCmd(3);
    for (int i=0 ; i < 4 ; i++) {			// give me some time to return by hand
      if (sol.query(FWI)) {				// request firware information
        SPrintFW(); 
        break; 
      } 
      delay(1000);
    }
  }
}


void doMP(){
  if (connect ) {     
    SPrintCmd(4);
    for (int i=0 ; i < 4 ; i++) {
      if (sol.query(RMP)) {				// request maximum power so far
        SPrintMP(); 
        break; 
      } 
      delay(1000);
    }
  }
}

void doDS(){
  if (connect) {                
    SPrintCmd(2);
    for (int i=0 ; i < 4 ; i++) {
      if (sol.query(DVS)) {				// request Device status       
        SPrintDS(); 
        if (sol.Flag != 0x00) {				// Print error flags
          SPrintflag(); 
        }
        break;
      }
      delay(1000);
    } 
  }
}

void doRMP(){
  if (connect) {  
    SPrintCmd(5); 
    sol.query(ZMP);					// Reset Maximum power register
  }
}


void doHD(){
  if (connect) {  
    SPrintCmd(6); 
    for (int i=0 ; i < 10 ; i++) {			// loop this for the last 10 days
      if (sol.query(HSD,i)) {			// Read history data  
        SPrintHD(); 
      }
    }
     Serial.println();
  }
}



void doX(){
  SPrintbuffer();  
}




