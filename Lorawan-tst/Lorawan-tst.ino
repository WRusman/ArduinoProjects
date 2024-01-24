#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "Wire.h"
#include "SHT2x.h"

//Set these OTAA parameters to match your app/node in TTN
uint8_t devEui[] = { 0xAE, 0x2F, 0x57, 0x34, 0x4C, 0x8D, 0x2F, 0x8D };
uint8_t appEui[] = { 0xC2, 0xC0, 0xF5, 0xEC, 0x63, 0x4C, 0x3A, 0x71 };
uint8_t appKey[] = { 0xC7, 0xF1, 0x47, 0x29, 0xCD, 0x99, 0xC2, 0x73, 0xDF, 0xB4, 0x59, 0x27, 0x17, 0xEA, 0x96, 0x02 };

uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

static uint8_t Battery=0;
static uint8_t temp=0;
static uint8_t hum=0;
static char LoRaData;

TimerEvent_t sleepTimer;
bool sleepTimerExpired;

SHT2x sht;

static void wakeUp()
{
  sleepTimerExpired=true;
}

static void lowPowerSleep(uint32_t sleeptime)
{
  sleepTimerExpired=false;
  TimerInit( &sleepTimer, &wakeUp );
  TimerSetValue( &sleepTimer, sleeptime );
  TimerStart( &sleepTimer );
  //Low power handler also gets interrupted by other timers
  //So wait until our timer had expired
  while (!sleepTimerExpired) lowPowerHandler();
  TimerStop( &sleepTimer );
}

void setup() {
	Serial.begin(115200);

 
  LoRaWAN.begin(LORAWAN_CLASS, ACTIVE_REGION);
  
  //Enable ADR
  LoRaWAN.setAdaptiveDR(true);

  while (1) {
    Serial.print("Joining... ");
    LoRaWAN.joinOTAA(appEui, appKey, devEui);
    if (!LoRaWAN.isJoined()) {
      //In this example we just loop until we're joined, but you could
      //also go and start doing other things and try again later
      Serial.println("JOIN FAILED! Sleeping for 30 seconds");
      lowPowerSleep(30000);
    } else {
      Serial.println("JOINED");
      break;
    }
  }
  sht.begin();
  uint8_t stat = sht.getStatus();
  Serial.print(stat, HEX);
  
}

void loop()
{

  Battery = getBatteryVoltage();
  Serial.print("Battery voltage (mv):");
  Serial.println(Battery);

  sht.read();
  Serial.print("\t");
  Serial.print(sht.getTemperature(), 1);
  Serial.print("\t");
  Serial.println(sht.getHumidity(), 1);


  LoRaData = char(getBatteryVoltage()) + char(sht.getTemperature()) + char(sht.getHumidity());
  if (LoRaWAN.send(1, &Battery, 1, false)) {
    Serial.println("Send OK");
  } else {
    Serial.println("Send FAILED");
  }


  lowPowerSleep(300000);      
}
