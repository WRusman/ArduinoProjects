#define clkPin 25         // Clock signal for 5821
#define oePin 26          // Not Output Enabled for 5821, 0=enabled, 1=disabled
#define dataPin 27        // Data In 5821
#define numCols 96        // 96 positions (95 leds) horizontally
#define numRows 7         // 7 rows of leds vertically
#define numChar 19        // 19 characters of 8 bytes wide per display
#define numDisplays 5     // 5 displays total daisy chained
#define row1Pin 16
#define row2Pin 17
#define row3Pin 18
#define row4Pin 19
#define row5Pin 21
#define row6Pin 22
#define row7Pin 23

int thread1Counter = 1;
int thread2Counter = 1;
int thread3Counter = 1;
int thread4Counter = 1;
int thread5Counter = 1;
int thread6Counter = 1;

byte rowBuffer[numChar][numRows];
byte rowPins[numRows] = {row1Pin,row2Pin,row3Pin,row4Pin,row5Pin,row6Pin,row7Pin,};
