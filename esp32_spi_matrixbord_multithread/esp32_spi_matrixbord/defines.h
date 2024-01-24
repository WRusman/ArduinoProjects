#define clkPin 18         // Clock signal for 5821
#define oePin 16          // Not Output Enabled for 5821, 0=enabled, 1=disabled
#define dataPin 23        // Data In 5821
#define row1Pin 13
#define row2Pin 4
#define row3Pin 27
#define row4Pin 26
#define row5Pin 25
#define row6Pin 33
#define row7Pin 32

#define numCol 96        // 96 positions (95 leds) horizontally
#define numRow 7         // 7 rows of leds vertically
#define numChar 19        // 19 characters of 8 bytes wide per display
#define numRegister 12    // 12 8-bit shift registers wide per display
#define numDisplay 8      // 8 displays total daisy chained

#define AP_NAME             "MatrixBord"
#define OTA_NAME            "Matrix"
#define OTA_PASSWD          "0XsaDOIIZrvVBHark7hAaRcU5g7F5YuY"


int thread1Counter = 1;
int thread2Counter = 1;
int thread3Counter = 1;
int thread4Counter = 1;
int thread5Counter = 1;
int thread6Counter = 1;
int thread7Counter = 1;
int thread8Counter = 1;
int thread9Counter = 1;
int thread10Counter = 1;
int updateCounter = 0;

byte rowBuffer[(numDisplay*numRegister)][numRow];
int textBuffer[(numDisplay*numChar)][numRow];
byte rowPins[numRow] = {row1Pin,row2Pin,row3Pin,row4Pin,row5Pin,row6Pin,row7Pin};
