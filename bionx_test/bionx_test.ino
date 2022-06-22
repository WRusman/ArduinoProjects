#include <mcp_can.h>
#define CAN0_INT 2                              // Set INT to pin 2

MCP_CAN CAN0(10);                               // Set CS to pin 10

void setup() {
  Serial.begin(115200);
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input

  if(CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
  {
    Serial.println("Error Initializing MCP2515...");
  }
}

void loop() {

  startSystem();
  delay(50000);

  stopSystem();
  delay(50000);

}

void startSystem(){
  byte data[4] = {0x00, 0x20, 0x00, 0x00};
  CAN0.sendMsgBuf(0x10, 0, 4, data);
  byte data2[4]={0x00, 0x21, 0x00, 0x01};
  CAN0.sendMsgBuf(0x10, 0, 4, data2);
  Serial.println("System enabled");
}

void stopSystem(){
  Serial.println("System disabled");
}
