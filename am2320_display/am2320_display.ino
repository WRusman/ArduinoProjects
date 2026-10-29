#include "Adafruit_Sensor.h"
#include <Adafruit_AM2320.h>
#include "lcdgfx.h"
#include "8662275129.h"

Adafruit_AM2320 am2320 = Adafruit_AM2320();
DisplaySH1106_128x64_I2C display(-1); // or (-1,{busId, addr, scl, sda, frequency})

float temperature, humidity;

void printTempHum()
{
    temperature = am2320.readTemperature();
    humidity = am2320.readHumidity();
    display.printFixed(10, 10,"T:", STYLE_NORMAL);    
    display.printFixed(35, 10,String(temperature).c_str(), STYLE_NORMAL);
    display.printFixed(100, 10,"C", STYLE_NORMAL);

    display.printFixed(10, 40,"H:", STYLE_NORMAL);
    display.printFixed(35, 40,String(humidity).c_str(), STYLE_NORMAL);
    display.printFixed(100, 40,"%", STYLE_NORMAL);
    
    Serial.print("Temperature : ");
    Serial.println(String(temperature).c_str());
    Serial.print("Humidity : ");
    Serial.println(String(humidity).c_str());
    Serial.println("");

}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  am2320.begin();
  display.begin();
  display.fill(0x00);
  display.setFixedFont(Russo_One13x11);
  printTempHum();
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  printTempHum();
  delay(5000);
}
