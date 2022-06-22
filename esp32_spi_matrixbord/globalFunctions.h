WiFiManager wifiManager;
OTAWrapper ota;


void blinkLedThread(){
  if ((thread9Counter % 2) > 0) {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN,LOW);
  }
  thread9Counter++;
  if (thread9Counter == 99){thread9Counter=1;} ;
}

void OTAThread(){
    ota.loop(); 
}
