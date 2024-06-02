#include <Wiimote.h>

Wiimote wiimote;
float  totalWeight, leftWeight, rightWeight;

void setup() {
  Serial.begin(115200);
  wiimote.init(wiimote_callback);
}

void loop() {
  wiimote.handle();
  if (totalWeight >10){
   printf("Left=%f Right=%f \n",leftWeight,rightWeight);  
   
  }
}

void wiimote_callback(wiimote_event_type_t event_type, uint16_t handle, uint8_t *data, size_t len) {
  static int connection_count = 0;
  if(event_type == WIIMOTE_EVENT_DATA){
    if(data[1]==0x34){
      float weight[4];
      wiimote.get_balance_weight(data, weight);
        totalWeight=weight[BALANCE_POSITION_TOP_RIGHT]+weight[BALANCE_POSITION_BOTTOM_RIGHT]+weight[BALANCE_POSITION_TOP_LEFT]+weight[BALANCE_POSITION_BOTTOM_LEFT];
        leftWeight=weight[BALANCE_POSITION_TOP_LEFT]+weight[BALANCE_POSITION_BOTTOM_LEFT];
        rightWeight=weight[BALANCE_POSITION_TOP_RIGHT]+weight[BALANCE_POSITION_BOTTOM_RIGHT];
    }
  }else if(event_type == WIIMOTE_EVENT_INITIALIZE){
    printf("  event_type=WIIMOTE_EVENT_INITIALIZE\n");
    wiimote.scan(true);
  }else if(event_type == WIIMOTE_EVENT_SCAN_START){
    printf("  event_type=WIIMOTE_EVENT_SCAN_START\n");
  }else if(event_type == WIIMOTE_EVENT_SCAN_STOP){
    printf("  event_type=WIIMOTE_EVENT_SCAN_STOP\n");
    if(connection_count==0){
      wiimote.scan(true);
    }
  }else if(event_type == WIIMOTE_EVENT_CONNECT){
    printf("  event_type=WIIMOTE_EVENT_CONNECT\n");
    wiimote.set_led(handle, 1<<connection_count);
    connection_count++;
  }else if(event_type == WIIMOTE_EVENT_DISCONNECT){
    printf("  event_type=WIIMOTE_EVENT_DISCONNECT\n");
    connection_count--;
    wiimote.scan(true);
  }else{
    printf("  event_type=%d\n", event_type);
  }
  delay(100);
}
