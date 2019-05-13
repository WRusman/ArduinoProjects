#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

bool debug = true;
#define SerialDebug(text)   Serial.print(text);
#define SerialDebugln(text) Serial.println(text);

char ssid[] = "WOMANETOMA24";             //  your network SSID (name)
char pass[] = "koekenbakkerknol";        // your network password

static const char ntpServerName[] = "nl.pool.ntp.org";
const int timeZone = 1;             // Central European Time
int DSTactive = 0;                  // Daylight Savings Time
unsigned int localPort = 8888;      // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48;     // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

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
RGB color1, color2, color3, color4, color5, color6;

WiFiUDP Udp;
time_t getNtpTime();

// define the ws2812 strip connected to 0 (pin 3)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(86, 0, NEO_GRB + NEO_KHZ800); 

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(200);
  delay(100);
  randomSeed(analogRead(1));
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // connect to the defined SSID
  WiFi.begin(ssid, pass);
  
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    // create an array of numbers and shuffle them
    int colornumbers[8] = {1,2,3,4,5,6,7,8} ;
    for (int i= 0; i< 8; i++)
    {
      int pos = random(8);
      int t = colornumbers[i];  
      colornumbers[i] = colornumbers[pos];
      colornumbers[pos] = t;
    }

    // Pick 6 colors from the array
    color1 = RGBColors[colornumbers[random(8)]];
    color2 = RGBColors[colornumbers[random(8)]];
    color3 = RGBColors[colornumbers[random(8)]];
    color4 = RGBColors[colornumbers[random(8)]];
    color5 = RGBColors[colornumbers[random(8)]];
    color6 = RGBColors[colornumbers[random(8)]];

  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  // initialize the ws2812 strip
  strip.setBrightness(30);  
  strip.begin();
  strip.show();
}


time_t getNtpTime()
{
  IPAddress ntpServerIP; 
  while (Udp.parsePacket() > 0) ; 
  Serial.println("Transmit NTP Request");
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

void sendNTPpacket(IPAddress &address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123); 
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void setleds(int startled, int stopled, RGB led_color) {
  for (int lednum = startled; lednum <= stopled; lednum++){
       strip.setPixelColor(lednum, led_color.r, led_color.g, led_color.b);
  }
}
  
void ShowTime() {
  uint8_t m,h;

setleds(0,86,black);    // Turn the comple strip black or "off";


  // Het = led 0-2
                setleds(0,2,color1 );  
  // is  = led 3-4
                setleds(3,4,color2);
  // nul = led 5-7
                setleds(5,6,color3 );


  m=minute()/5;
  switch(m) {
    case 0:     // Uur = led 79-81
                Serial.print("Uur"); 
                setleds(79,81, color5);
                h = 0;                
      break;
    case 1:     // Vijf = led 12-15
                setleds(12,15,color4);    
                // Over = led 21-24
                setleds(21,24,color5);
                Serial.print("Vijf over ");
                h = 0;
      break;
    case 2:     // Tien = led 8-11
                setleds(8,11,color4);    
                // Over = led 21-24
                setleds(21,24,color5);                
                Serial.print("Tien over ");                
                h = 0;                
      break;
    case 3:     // Kwart = led 16-20
                setleds(16,20,color4);    
                // Over = led 21-24
                setleds(21,24,color5);                
                Serial.print("Kwart over ");                
                h = 0;                
      break;
    case 4:     // Tien = led 8-11
                setleds(8,11,color4);    
                // Voor = led 29-32
                setleds(29,32,color5);                
                // Half = led 25-28
                setleds(25,28,color6);                
                Serial.print("Tien voor half ");  
                h = 1;              
      break;
    case 5:     // Vijf = led 12-15
                setleds(12,15,color4);    
                // Voor = led 29-32
                setleds(29,32,color5);                
                // Half = led 25-28
                setleds(25,28,color6);                
                Serial.print("Vijf voor half ");                
                h = 1;              
      break;
    case 6:     // Half = led 25-28
                setleds(25,28,color4);    
                Serial.print("half ");
                h = 1;                              
      break;
    case 7:     // Vijf = led 12-15
                setleds(12,15,color4);    
                // Over = led 21-24
                setleds(21,24,color5);                
                // Half = led 25-28
                setleds(25,28,color6);                
                Serial.print("Vijf over half ");                
                h = 1;                              
      break;
    case 8:     // Tien = led 8-11
                setleds(8,11,color4);    
                // Over = led 21-24
                setleds(21,24,color5);                
                // Half = led 25-28
                setleds(25,28,color6);                
                Serial.print("tien over half ");                
                h = 1;                              
      break;
    case 9:     // Kwart = led 16-20
                setleds(16,20,color4);    
                // Voor = led 29-32
                setleds(29,32,color5);                
                Serial.print("kwart voor ");                
                h = 1;                              
      break;
    case 10:    // Tien = led 8-11
                setleds(8,11,color4);
                // Voor = led 29-32
                setleds(29,32,color5);                
                Serial.print("tien voor ");                
                h = 1;                              
      break;
    case 11:    // Vijf = led 12-15
                setleds(12,15,color4);    
                // Voor = led 29-32
                setleds(29,32,color5);                
                Serial.print("Vijf voor ");                
                h = 1;                              
      break;
  }

  h += hour();
  h += DSTactive;
  switch (h) {
    case 0:
    case 12:
    case 24:     // Twaalf = led 62-67
                setleds(62,67,white) ;    
                 Serial.print("Twaalf");                
      break;
    case 1:
    case 13:    // Een = led 72-74
                setleds(72,74,white) ;    
                 Serial.print("Een");                    
      break;
    case 2:
    case 14:    // Twee = led 36-39
                setleds(36,39,white) ;    
                 Serial.print("Twee");                    
      break;
    case 3:
    case 15:    // Drie = led 49-52
                setleds(49,52,white) ;    
                 Serial.print("Drie");                    
      break;
    case 4:
    case 16:    // Vier = led 68-71
                setleds(68,71,white) ;    
                 Serial.print("Vier");                    
      break;
    case 5:
    case 17:    // Vijf = led 75-78
                setleds(75,78,white) ;    
                 Serial.print("Vijf");                    
      break;
    case 6:
    case 18:    // Zes = led 40-42
                setleds(40,42,white) ;    
                 Serial.print("Zes");                    
      break;
    case 7:
    case 19:    // Zeven = led 53-57
                setleds(53,57,white) ;    
                 Serial.print("Zeven");                    
      break;
    case 8:
    case 20:    // Acht = led 33-36
                setleds(33,36,white) ;    
                 Serial.print("Acht");                    
      break;
    case 9:
    case 21:    // Negen = led 57-61
                setleds(57,61,white) ;    
                 Serial.print("Negen");                    
      break;
    case 10:
    case 22:    // Tien = led 43-46
                setleds(43,46,white) ;    
                 Serial.print("Tien");                    
      break;
    case 11:
    case 23:    // Elf = led 47-49
                setleds(47,49,white) ;    
                 Serial.print("Elf");                    
      break;

  }

  m=minute()%5; // minute dots = led 
                Serial.print(", ");
                Serial.print(m);
                Serial.print(" extra minuten");
                
  switch(m) {
    case 1:     // dots = 85
                setleds(85,85, color5);
      break;
    case 2:     // dots = 84-85
                setleds(84,85, color5);
      break;
    case 3:     // dots = 83-85
                setleds(83,85, color5);
      break;
    case 4:     // dots = 82-85
                setleds(82,85, color5);
      break;
  }
  
  Serial.println(); 
  strip.show();
}

void checkDST(){
      int beginDSTDate=  (31 - (5* year() /4 + 4) % 7);
      int beginDSTMonth=3;  //last sunday of october
      int endDSTDate= (31 - (5 * year() /4 + 1) % 7);
      int endDSTMonth=10;
      // DST is valid as:
      if (((month() > beginDSTMonth) && (month() < endDSTMonth))
        || ((month() == beginDSTMonth) && (day() >= beginDSTDate))
        || ((month() == endDSTMonth) && (day() <= endDSTDate)))
        {
          Serial.println("DST"); 
          DSTactive=1;
        }
        else
        {
          Serial.println("No DST");         
          DSTactive=0;
        }
}

void loop() {
    checkDST();
    ShowTime();
    delay(5000);
    
}
