#include "Adafruit_Sensor.h"
#include <Adafruit_AM2320.h>

Adafruit_AM2320 am2320 = Adafruit_AM2320();


int value = 0;
float temperature, humidity;
long lastMsg = 0;

void setup() {
  randomSeed(micros());
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  am2320.begin();
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;
    temperature = am2320.readTemperature();
    humidity = am2320.readHumidity();
    Serial.print("Temperature : ");
    Serial.println(String(temperature).c_str());
    Serial.print("Humidity : ");
    Serial.println(String(humidity).c_str());
    Serial.println("");

}
}
