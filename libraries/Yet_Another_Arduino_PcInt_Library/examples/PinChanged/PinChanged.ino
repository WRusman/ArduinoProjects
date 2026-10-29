/*
 * This example shows how to monitor state changes on a pin
 */

#include <YetAnotherPcInt.h>
#define ARRAY_SIZE(X) (sizeof(X)/sizeof(*X))


struct Pin {
  int number;
  const char* name;
};


// List of pins we are monitoring
Pin monitored_pins[] = {
  {A8,  "A8"},
  {A9,  "A9"},
  {A10, "A10"},
  {A11, "A11"},
  {A12, "A12"},
  {A13, "A13"},
  {A14, "A14"},
  {A15, "A15"},
};


void pinChanged(Pin* pin, bool pinstate) {
  Serial.print("Pin ");
  Serial.print(pin->name);
  Serial.print(" (");
  Serial.print(pin->number);
  Serial.print(") changed to ");
  Serial.println(pinstate ? "HIGH" : "LOW");
}


void setup() {
  Serial.begin(115200);
  
  //Tell each pin to start listening
  for (int i=0; i<ARRAY_SIZE(monitored_pins); i++) {
    pinMode(monitored_pins[i].number, INPUT_PULLUP);
    PcInt::attachInterrupt(monitored_pins[i].number, pinChanged, &monitored_pins[i], CHANGE);
  }
}

void loop() {}