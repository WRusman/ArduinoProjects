/*
 * This example shows how to develop a component which uses Pin Change Interruptions
 */

#include <YetAnotherPcInt.h>
#define ARRAY_SIZE(X) (sizeof(X)/sizeof(*X))


//Component that display changes on a pin
struct PinListener {
  int number;
  const char* name;
  
  void begin() {
    pinMode(number, INPUT_PULLUP);
    PcInt::attachInterrupt(number, PinListener::changed, this, CHANGE);
  }
  
  void end() {
    PcInt::detachInterrupt(number);
  }
  
private:
  void changed(bool pinstate) {
    Serial.print("Pin ");
    Serial.print(name);
    Serial.print(" (");
    Serial.print(number);
    Serial.print(") changed to ");
    Serial.println(pinstate ? "HIGH" : "LOW");
  }
  static void changed(PinListener* _this, bool pinstate) {
    _this->changed(pinstate);
  }
};


//List of pins we are listening to
PinListener monitored_pins[] = {
  {A8,  "A8"},
  {A9,  "A9"},
  {A10, "A10"},
  {A11, "A11"},
  {A12, "A12"},
  {A13, "A13"},
  {A14, "A14"},
  {A15, "A15"},
};


void setup() {
  Serial.begin(115200);
  
  //Tell each pin to start listening
  for (int i=0; i<ARRAY_SIZE(monitored_pins); i++) {
    monitored_pins[i].begin();
  }
}

void loop() {}
