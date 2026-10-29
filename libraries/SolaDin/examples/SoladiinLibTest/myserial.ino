void SPrintCmd(int cmd) {
  switch (cmd) {
  case 1:
    Serial.println("Cmd: Probe     ");
    break;
  case 2:
    Serial.println("Cmd: Devstate  ");
    break;
  case 3:
    Serial.println("Cmd: Firmware  ");
    break;    
  case 4:
    Serial.println("Cmd: MaxPower  ");
    break;   
  case 5:
    Serial.println("Cmd: Reset MaxPower  ");
    break;  
  case 6:
    Serial.println("Cmd: History Data  ");
    break; 
  }   
}



void SPrintHD() {
  Serial.print("Opartion Time= ");
  char timeStr[14];
  sprintf(timeStr, "%02d:%02d hh:mm ",(sol.DailyOpTm/12), ((sol.DailyOpTm*5)%12));
  Serial.print(timeStr); 
  Serial.print(float(sol.Gridoutput)/100);
  Serial.println(" kWh");
} 


void SPrintFW() {
  Serial.print("FW ID= ");
  Serial.println(byte(sol.FW_ID),HEX);
  Serial.print("Ver= ");
  Serial.println(word(sol.FW_version),HEX);
  Serial.print("Date= ");
  Serial.println(word(sol.FW_date),HEX);
  Serial.println();
} 

void SPrintMP() {
  Serial.print("MaxPower= ");
  Serial.print(word(sol.MaxPower));
  Serial.println(" watt");
  Serial.println();
}

void SPrintDS() {
  Serial.print("PV= ");
  Serial.print(float(sol.PVvolt)/10);
  Serial.print("V;   ");
  Serial.print(float(sol.PVamp)/100);
  Serial.println("A");

  Serial.print("AC= ");
  Serial.print(sol.Gridpower);
  Serial.print("W;  ");
  Serial.print(float(sol.Gridfreq)/100);
  Serial.print("Hz;  ");
  Serial.print(sol.Gridvolt);
  Serial.println("Volt");
  
  Serial.print("Device Temperature= ");
  Serial.print(sol.DeviceTemp);
  Serial.println(" Celcius");
  
  Serial.print("AlarmFlag= ");
  Serial.println(sol.Flag,BIN);
  
  Serial.print("Total Power= ");
  Serial.print(float(sol.Totalpower)/100);
  Serial.println("kWh");
  // I really don't know, wy i must split the sprintf ?
  Serial.print("Total Operating time= ");
  char timeStr[14];
  sprintf(timeStr, "%04d:",(sol.TotalOperaTime/60));
  Serial.print(timeStr); 
  sprintf(timeStr, "%02d hh:mm ",  (sol.TotalOperaTime%60));
  Serial.println(timeStr);
  Serial.println();  
 }


void SPrintflag(){
  if ( sol.Flag & 0x0001 ){
    Serial.println("Usolar too high"); 
  }
  if( sol.Flag & 0x0002 ){
    Serial.println("Usolar too low"); 
  }
  if( sol.Flag & 0x0004 ){
    Serial.println("No Grid"); 
  }
  if( sol.Flag & 0x0008 ){
    Serial.println("Uac too high"); 
  }   
  if( sol.Flag & 0x0010 ){
    Serial.println("Uac too low"); 
  }      
  if( sol.Flag & 0x0020 ){
    Serial.println("Fac too high"); 
  }      
  if( sol.Flag & 0x0040 ){
    Serial.println("Fac too low"); 
  }  
  if( sol.Flag & 0x0080 ){
    Serial.println("Temperature to high"); 
  }  
  if( sol.Flag & 0x0100 ){
    Serial.println("Hardware failure"); 
  } 
  if( sol.Flag & 0x0200 ){
    Serial.println("Starting"); 
  }   
  if( sol.Flag & 0x0400 ){
    Serial.println("Max Power"); 
  }   
  if( sol.Flag & 0x0800 ){
    Serial.println("Max current"); 
  }      
}   

void SPrintbuffer(){
  Serial.print("0x");
  for (int i=0; i<sol.RxLgth ; i++) {
    if (byte(sol.RxBuf[i]) < 0x10) {
      Serial.print("0");
    }
    Serial.print(byte(sol.RxBuf[i]),HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("BufferSize= ");
  Serial.println(sol.RxLgth);
  Serial.println();  
}



