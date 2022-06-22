WiFiManager wifiManager;
OTAWrapper ota;


void blinkLedThread(){
  if ((thread6Counter % 2) > 0) {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN,LOW);
  }
  thread6Counter++;
  if (thread6Counter == 99){thread6Counter=1;} ;
}

void OTAThread(){

}
