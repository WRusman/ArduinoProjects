#include <ESP8266WiFi.h>          
#include <WiFiManager.h>          
#include <Ticker.h>
#include <Timezone.h>             //https://github.com/JChristensen/Timezone
#include <NTPClient.h>            //https://github.com/arduino-libraries/NTPClient
#include <Adafruit_NeoPixel.h>

Ticker ticker;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Create a structure to store colors in
struct RGB {                       
   byte r;
   byte g;
   byte b;
};

// create defintions of colors to be used
RGB red = { 255 , 145 , 156 };
RGB blue = { 166 , 94 , 255};
RGB yellow = { 255 , 242 , 120 };
RGB green = { 133 , 255 , 153 };
RGB purple = { 222 , 111 , 208 };
RGB orange = { 255 , 217 , 63 };
RGB darkblue = { 63 , 90 , 255 };
RGB darkgreen = { 67 , 179 , 64 };
RGB black = { 0 , 0 , 0 };
RGB white = { 255 , 255 , 255 };
RGB RGBColors[8] = {red, blue, yellow, green, purple, orange, darkblue, darkgreen};
RGB color1, color2, color3, color4, color5, color6, color7;

// define the ws2812 strip connected to 0 (pin 3)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(85, 0, NEO_GRB + NEO_KHZ800); 

TimeChangeRule nlDST = {"DST", Last, Sun, Mar, 1, +120};  //UTC + 2 hours
TimeChangeRule nlSTD = {"STD", Last, Sun, Oct, 1, +60};   //UTC + 1 hour
Timezone MyTZ(nlDST, nlSTD);

int colorTimer = 0;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(1, INPUT);
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);

  randomSeed(analogRead(1));

  ticker.attach(0.6, tick);
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  if (!wm.autoConnect("woordklok")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, HIGH);
  timeClient.begin();

  // initialize the ws2812 strip
  strip.setBrightness(30);  
  strip.begin();
  setleds(0,84,black);    // Turn the comple strip black or "off";  
  strip.show();
  randomizeColors();

}


void loop() {
  timeClient.update();  
  time_t local_time = MyTZ.toLocal(timeClient.getEpochTime());
  showTime(local_time);
  delay(5000);
}

void tick()
{
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

void setleds(int startled, int stopled, RGB led_color) {
  for (int lednum = startled; lednum <= stopled; lednum++){
       strip.setPixelColor(lednum, led_color.r, led_color.g, led_color.b);
  }
}
  
void randomizeColors(){
  Serial.println("kleur husselen");
  int colornumbers[8] = {0,1,2,3,4,5,6,7} ;
  for (int i= 0; i< 8; i++)
  {
    int pos = random(8);
    int t = colornumbers[i];  
    colornumbers[i] = colornumbers[pos];
    colornumbers[pos] = t;
  }

  color1 = RGBColors[colornumbers[random(8)]];
  color2 = RGBColors[colornumbers[random(8)]];
  color3 = RGBColors[colornumbers[random(8)]];
  color4 = RGBColors[colornumbers[random(8)]];
  color5 = RGBColors[colornumbers[random(8)]];
  color6 = RGBColors[colornumbers[random(8)]];
  color7 = RGBColors[colornumbers[random(8)]];  
  colorTimer=0;
}

void showTime(time_t timedate) {
  uint8_t m,h;

  setleds(0,84,black);    // Turn the comple strip black or "off";


  // Het = led 0-2
                setleds(0,2,white );  
  // is  = led 3-4
                setleds(3,4,color2);
  // nu = led 5-6
                setleds(5,6,color3 );

  m=minute(timedate)/5;
  
  switch(m) {
    case 0:     // Uur = led 78-80
                Serial.print("Heel Uur "); 
                setleds(78,80, color5);
                h = 0;                
      break;
    case 1:     // Vijf = led 11-14
                setleds(11,14,color4);    
                // Over = led 20-23
                setleds(20,23,color5);
                Serial.print("Vijf over ");
                h = 0;
      break;
    case 2:     // Tien = led 7-10
                setleds(7,10,color4);    
                // Over = led 20-23
                setleds(20,23,color5);
                Serial.print("Tien over ");                
                h = 0;                
      break;
    case 3:     // Kwart = led 15-19
                setleds(15,19,color4);    
                // Over = led 20-23
                setleds(20,23,color5);                
                Serial.print("Kwart over ");                
                h = 0;                
      break;
    case 4:     // Tien = led 7-10
                setleds(7,10,color4);    
                // Voor = led 28-31
                setleds(28,31,color5);                
                // Half = led 24-27
                setleds(24,27,color6);                
                Serial.print("Tien voor half ");  
                h = 1;              
      break;
    case 5:     // Vijf = led 11-14
                setleds(11,14,color4);    
                // Voor = led 28-31
                setleds(28,31,color5);                
                // Half = led 24-27
                setleds(24,27,color6);                
                Serial.print("Vijf voor half ");                
                h = 1;              
      break;
    case 6:     // Half = led 24-27
                setleds(24,27,color6);                
                Serial.print("half ");
                h = 1;                              
      break;
    case 7:     // Vijf = led 11-14
                setleds(11,14,color4);    
                // Over = led 20-23
                setleds(20,23,color5);
                // Half = led 24-27
                setleds(24,27,color6);                
                Serial.print("Vijf over half ");                
                h = 1;                              
      break;
    case 8:     // Tien = led 7-10
                setleds(7,10,color4);    
                // Over = led 20-23
                setleds(20,23,color5);
                // Half = led 24-27
                setleds(24,27,color6);                
                Serial.print("tien over half ");                
                h = 1;                              
      break;
    case 9:     // Kwart = led 15-19
                setleds(15,19,color4);    
                // Voor = led 28-31
                setleds(28,31,color5);                
                Serial.print("kwart voor ");                
                h = 1;                              
      break;
    case 10:    // Tien = led 7-10
                setleds(7,10,color4);    
                // Voor = led 28-31
                setleds(28,31,color5);                
                Serial.print("tien voor ");                
                h = 1;                              
      break;
    case 11:    // Vijf = led 11-14
                setleds(11,14,color4);    
                // Voor = led 28-31
                setleds(28,31,color5);                
                Serial.print("Vijf voor ");                
                h = 1;                              
      break;
  }

  h += hour(timedate);
  switch (h) {
    case 0:
    case 12:
    case 24:    // Twaalf = led 61-66
                setleds(61,66,color7) ;    
                 Serial.print("Twaalf");                
      break;
    case 1:
    case 13:    // Een = led 71-73
                setleds(71,73,color7) ;    
                 Serial.print("Een");                    
      break;
    case 2:
    case 14:    // Twee = led 35-38
                setleds(35,38,color7) ;    
                 Serial.print("Twee");                    
      break;
    case 3:
    case 15:    // Drie = led 48-51
                setleds(48,51,color7) ;    
                 Serial.print("Drie");                    
      break;
    case 4:
    case 16:    // Vier = led 67-70
                setleds(67,70,color7) ;    
                 Serial.print("Vier");                    
      break;
    case 5:
    case 17:    // Vijf = led 74-77
                setleds(74,77,color7) ;    
                 Serial.print("Vijf");                    
      break;
    case 6:
    case 18:    // Zes = led 39-41
                setleds(39,41,color7) ;    
                 Serial.print("Zes");                    
      break;
    case 7:
    case 19:    // Zeven = led 52-56
                setleds(52,56,color7) ;    
                 Serial.print("Zeven");                    
      break;
    case 8:
    case 20:    // Acht = led 32-35
                setleds(32,35,color7) ;    
                 Serial.print("Acht");                    
      break;
    case 9:
    case 21:    // Negen = led 56-60
                setleds(56,60,color7) ;    
                 Serial.print("Negen");                    
      break;
    case 10:
    case 22:    // Tien = led 42-45
                setleds(42,45,color7) ;    
                 Serial.print("Tien");                    
      break;
    case 11:
    case 23:    // Elf = led 45-47
                setleds(46,48,color7) ;    
                 Serial.print("Elf");                    
      break;

  }

  m=minute(timedate)%5; // minute dots = led 
                Serial.print(", ");
                Serial.print(m);
                Serial.println(" extra minuten");
                
  switch(m) {
    case 1:     // dots = 84
                setleds(84,84, color4);
      break;
    case 2:     // dots = 83-84
                setleds(83,84, color4);
      break;
    case 3:     // dots = 82-84
                setleds(82,84, color4);
      break;
    case 4:     // dots = 81-84
                setleds(81,84, color4);
                colorTimer++;
                if (colorTimer == 11){
                  randomizeColors();
                }
      break;
      }
    
  strip.show();
}
