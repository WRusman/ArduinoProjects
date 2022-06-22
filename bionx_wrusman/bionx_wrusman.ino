#include <mcp_can.h>

#define CAN0_INT 2                              // Set INT to pin 2
#define Battery   0x10
#define Motor     0x20
#define Screen    0x08

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];      

MCP_CAN CAN0(10);                               // Set CS to pin 10

void setup() {
  
  Serial.begin(115200);
  
  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input

  if(CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
  {
    Serial.println("Error Initializing MCP2515...");
  }
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
}

void loop() {

  startSystem();
  delay(10000);

  stopSystem();
  delay(10000);

}

void setRegisterValue(byte CanID, byte part1, byte part2, byte part3, byte part4){
  byte message[4] = {0x00, 0x00, 0x00, 0x00};
  message[0] = part1;
  message[1] = part2;
  message[2] = part3;
  message[3] = part4; 
  byte sndStat = CAN0.sendMsgBuf(CanID, 0, 4, message);
    if((sndStat == CAN_OK) or (sndStat == CAN_SENDMSGTIMEOUT)){
    Serial.print("Message Sent ID: ");
    sprintf(msgString, " 0x%.2X", CanID);
    Serial.print(msgString);    
    Serial.print(" Data: ");
      for(byte i = 0; i<4; i++){
        sprintf(msgString, " 0x%.2X", message[i]);
        Serial.print(msgString);
        }
        Serial.println();
      } else {
    Serial.println("Error Sending Message...");
  }
    delay(100);   // send data per 100ms
}

void getRegisterValue(byte CanID, byte part1, byte part2){
  bool valueReceived = false;
  byte message[2] = {0x00, 0x00};
  message[0] = part1;
  message[1] = part2;

  byte sndStat = CAN0.sendMsgBuf(CanID, 0, 2, message);
    if((sndStat == CAN_OK) or (sndStat == CAN_SENDMSGTIMEOUT)){
    Serial.print("Message Sent ID: ");
    sprintf(msgString, " 0x%.2X", CanID);
    Serial.print(msgString);    
    Serial.print(" Data: ");
      for(byte i = 0; i<2; i++){
        sprintf(msgString, " 0x%.2X", message[i]);
        Serial.print(msgString);
        }
        Serial.println();
      } else {
    Serial.println("Error Sending Message...");
  }

 while (!valueReceived){
   if(!digitalRead(CAN0_INT)){                  // If CAN0_INT pin is low, read receive buffer
     Serial.print("message received");
     CAN0.readMsgBuf(&rxId, &len, rxBuf);       // Read data: len = data length, buf = data byte(s 

      sprintf(msgString, " ID: 0x%.3lX     DLC: %1d  Data:", rxId, len);
      Serial.print(msgString);

        Serial.print(",");
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
        //Serial.print(rxBuf[i],HEX);
      } 
     valueReceived=true;
    }
  }
  Serial.println();
}

void startSystem(){
  setRegisterValue(Battery, 0x00, 0x20, 0x00, 0x00); // charger status ?
  setRegisterValue(Battery, 0x00, 0x21, 0x00, 0x01); // CONFIG_POWER_VOLTAGE_ENABLE, 1 to enable vPower
  getRegisterValue(Battery, 0x00, 0x3B); // get Battery hardware version 
  getRegisterValue(Battery, 0x00, 0x3C); // get Battery software version
  setRegisterValue(Battery, 0x00, 0x22, 0x00, 0x00); // no battery accessory enabled
  getRegisterValue(Battery, 0x00, 0x3D); // get Battery config type
  setRegisterValue(Motor, 0x00, 0x02, 0x00, 0x00);
  setRegisterValue(Motor, 0x00, 0x41, 0x00, 0x00); // motor enable
  setRegisterValue(Motor, 0x00, 0x42, 0x00, 0x01); // direction forward
  getRegisterValue(Motor, 0x00, 0x20); // get motor software version
  getRegisterValue(Motor, 0x00, 0x6C); // get motor torque gauge type
  getRegisterValue(Motor, 0x00, 0x11); // get motor speed status
  setRegisterValue(Motor, 0x00, 0xA5, 0x00, 0xAA); // write motor unlock
  setRegisterValue(Motor, 0x00, 0x7A, 0x00, 0x75); // ?? write gauge joint
  setRegisterValue(Motor, 0x00, 0x7B, 0x00, 0x30); // ?? write min throttle value
  setRegisterValue(Motor, 0x00, 0x7C, 0x00, 0x17); // ?? write 
  setRegisterValue(Motor, 0x00, 0x7D, 0x00, 0x70); // ?? write max throttle value
  getRegisterValue(Motor, 0x00, 0x21); // get motor TORQUE_GAUGE_VALUE 
  setRegisterValue(Motor, 0x00, 0x8B, 0x00, 0x32); // ?? write max assist speed
  
  Serial.println("System enabled");
}

void stopSystem(){
  setRegisterValue(Battery, 0x00, 0x21, 0x00, 0x00); // CONFIG_POWER_VOLTAGE_ENABLE, 0 to disable vPower
  setRegisterValue(Battery, 0x00, 0x25, 0x00, 0x01); // write BATTERY_CONFIG_SHUTDOWN, 1 to shutdown 
  Serial.println("System disabled");
}
