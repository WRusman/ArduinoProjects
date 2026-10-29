#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(109, 22, NEO_GRB + NEO_KHZ800); // pin 22

struct RGB {                       
   byte r;
   byte g;
   byte b;
};

RGB red = { 255 , 0 , 0 };
RGB orange = { 255 , 100 , 0 };
RGB yellow = { 255 , 255 , 0 };
RGB black = { 0 , 0 , 0 };
RGB white = { 255 , 255 , 255 };
RGB blue = { 0 , 0 , 255 };


void setleds(int startled, int stopled, RGB led_color) {
  for (int lednum = startled; lednum <= stopled; lednum++){
       strip.setPixelColor(lednum, led_color.r, led_color.g, led_color.b);
  }
}

void startLedStrip(){
  strip.setBrightness(100);  
  strip.begin();
  setleds(0,108,white);    // Turn the complete strip white or "0:00:00";  
  strip.show();
}


//    3  4  5            26 27 28      47 48 49          70 71 72      91 92 93
//  2          6       25        29  46        50      69        73  90        94
//  1          7  21   24        30  45        51  65  68        74  89        95
//  0          8       23        31  44        52      67        75  88        96
//    11 10  9           34 33 32      55 54 53          78 77 76      99 98 97
//  12        20       35        43  56        64      79        87  100       108
//  13        19  22   36        42  57        63  66  80        86  101       107
//  14        18       37        41  58        62      81        85  102       106
//    15 16 17           38 39 40      59 60 61          82 83 84     103 104 105


void outputDisplayHour(int countDownHour, RGB clockColor){

  switch (countDownHour) {
    case -1:    // - = led 9-11
                setleds(9,11,clockColor) ;    
    break;
    case 0:     // 0 = led 0-8 en 12-20
                setleds(0,8,clockColor) ;    
                setleds(12,20,clockColor) ;    
    break;
    case 1:     // 1 = led 6-8 en 18-20
                setleds(6,8,clockColor) ;    
                setleds(18,20,clockColor) ;    
    break;
    case 2:     // 2 = led 3-17
                setleds(3,17,clockColor) ;    
    break;
    case 3:     // 3 = led 3-11 en 15-20
                setleds(3,11,clockColor) ;    
                setleds(15,20,clockColor) ;    
    break;
    case 4:     // 4 = led 0-2, 6-11 en 18-20
                setleds(0,2,clockColor) ;    
                setleds(6,11,clockColor) ;    
                setleds(18,20,clockColor) ;                    
    break;
    case 5:     // 5 = led 0-5, 9-11 en 15-20
                setleds(0,5,clockColor) ;    
                setleds(9,11,clockColor) ;    
                setleds(15,20,clockColor) ;    
    break;
    case 6:     // 6 = led 0-5 en 9-20
                setleds(0,5,clockColor) ;    
                setleds(9,20,clockColor) ;    
    break;
    case 7:     // 7 = led 3-8 en 18-20
                setleds(3,8,clockColor) ;    
                setleds(18,20,clockColor) ;    
    break;
    case 8:     // 8 = led 0-20
                setleds(0,20,clockColor) ;    
    break;
    case 9:     // 9 = led 0-11 en 15-20
                setleds(0,11,clockColor) ;    
                setleds(15,20,clockColor) ;    
    break;
  }
  if (countDownHour >= 0) setleds(21,22,clockColor) ;    
  
}  

void outputDisplayMinutes(uint8_t countDownMinutes, RGB clockColor){

  switch (countDownMinutes/10) {
    case 0:     // 0 = led 23-31 en 35-43
                setleds(23,31,clockColor) ;    
                setleds(35,43,clockColor) ;    
    break;
    case 1:     // 1 = led 29-31 en 41-43
                setleds(29,31,clockColor) ;    
                setleds(41,43,clockColor) ;    
    break;
    case 2:     // 2 = led 26-40
                setleds(26,40,clockColor) ;    
    break;
    case 3:     // 3 = led 26-34 en 38-43
                setleds(26,34,clockColor) ;    
                setleds(38,43,clockColor) ;    
    break;
    case 4:     // 4 = led 23-25, 29-34 en 41-43
                setleds(23,25,clockColor) ;    
                setleds(29,34,clockColor) ;    
                setleds(41,43,clockColor) ;                    
    break;
    case 5:     // 5 = led 23-28, 32-34 en 38-43
                setleds(23,28,clockColor) ;    
                setleds(32,34,clockColor) ;    
                setleds(38,43,clockColor) ;    
    break;
  }

  switch (countDownMinutes %10) {
    case 0:     // 0 = led 44-52 en 56-64
                setleds(44,52,clockColor) ;    
                setleds(56,64,clockColor) ;    
    break;
    case 1:     // 1 = led 50-52 en 62-64
                setleds(50,52,clockColor) ;    
                setleds(62,64,clockColor) ;    
    break;
    case 2:     // 2 = led 47-61
                setleds(47,61,clockColor) ;    
    break;
    case 3:     // 3 = led 47-55 en 59-64
                setleds(47,55,clockColor) ;    
                setleds(59,64,clockColor) ;    
    break;
    case 4:     // 4 = led 44-46, 50-55 en 62-64
                setleds(44,46,clockColor) ;    
                setleds(50,55,clockColor) ;    
                setleds(62,64,clockColor) ;                    
    break;
    case 5:     // 5 = led 44-49, 53-55 en 59-64
                setleds(44,49,clockColor) ;    
                setleds(53,55,clockColor) ;    
                setleds(59,64,clockColor) ;    
    break;
    case 6:     // 6 = led 44-49 en 53-64
                setleds(44,49,clockColor) ;    
                setleds(53,64,clockColor) ;    
    break;
    case 7:     // 7 = led 47-52 en 62-64
                setleds(47,52,clockColor) ;    
                setleds(62,64,clockColor) ;    
    break;
    case 8:     // 8 = led 44-64
                setleds(44,64,clockColor) ;    
    break;
    case 9:     // 9 = led 44-55 en 59-64
                setleds(44,55,clockColor) ;    
                setleds(59,64,clockColor) ;    
    break;
  }
  setleds(65,66,clockColor) ;    
}  

void outputDisplaySeconds(uint8_t countDownSeconds, RGB clockColor){

  switch (countDownSeconds/10) {
    case 0:     // 0 = led 67-75 en 79-87
                setleds(67,75,clockColor) ;    
                setleds(79,87,clockColor) ;    
    break;
    case 1:     // 1 = led 73-75 en 85-87
                setleds(73,75,clockColor) ;    
                setleds(85,87,clockColor) ;    
    break;
    case 2:     // 2 = led 70-84
                setleds(70,84,clockColor) ;    
    break;
    case 3:     // 3 = led 70-78 en 82-87
                setleds(70,78,clockColor) ;    
                setleds(82,87,clockColor) ;    
    break;
    case 4:     // 4 = led 67-69, 73-78 en 85-87
                setleds(67,69,clockColor) ;    
                setleds(73,78,clockColor) ;    
                setleds(85,87,clockColor) ;                    
    break;
    case 5:     // 5 = led 67-72, 76-78 en 82-87
                setleds(67,72,clockColor) ;    
                setleds(76,78,clockColor) ;    
                setleds(82,87,clockColor) ;    
    break;
  }

  switch (countDownSeconds %10) {
    case 0:     // 0 = led 88-96 en 100-108
                setleds(88,96,clockColor) ;    
                setleds(100,108,clockColor) ;    
    break;
    case 1:     // 1 = led 94-96 en 106-108
                setleds(94,96,clockColor) ;    
                setleds(106,108,clockColor) ;    
    break;
    case 2:     // 2 = led 91-105
                setleds(91,105,clockColor) ;    
    break;
    case 3:     // 3 = led 91-99 en 103-108
                setleds(91,99,clockColor) ;    
                setleds(103,108,clockColor) ;    
    break;
    case 4:     // 4 = led 88-90, 94-99 en 106-108
                setleds(88,90,clockColor) ;    
                setleds(94,99,clockColor) ;    
                setleds(106,108,clockColor) ;                    
    break;
    case 5:     // 5 = led 88-93, 97-99 en 103-108
                setleds(88,93,clockColor) ;    
                setleds(97,99,clockColor) ;    
                setleds(103,108,clockColor) ;    
    break;
    case 6:     // 6 = led 88-93 en 97-108
                setleds(88,93,clockColor) ;    
                setleds(97,108,clockColor) ;    
    break;
    case 7:     // 7 = led 91-96 en 106-108
                setleds(91,96,clockColor) ;    
                setleds(106,108,clockColor) ;    
    break;
    case 8:     // 8 = led 88-108
                setleds(88,108,clockColor) ;    
    break;
    case 9:     // 9 = led 88-99 en 103-108
                setleds(88,99,clockColor) ;    
                setleds(103,108,clockColor) ;    
    break;
  }
}  


void showTime(int displayHours, uint8_t displayMinutes, uint8_t displaySeconds, RGB clockColor){
  setleds(0,108,black);    // Turn the complete strip black or "off";  
  outputDisplayHour(displayHours,clockColor);
  outputDisplayMinutes(displayMinutes,clockColor);
  outputDisplaySeconds(displaySeconds,clockColor);  
  strip.show();
}
