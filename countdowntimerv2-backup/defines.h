#define matrixClkPin 18       // VSPICLK Clock signal for Matrix display
#define matrixOePin 5         // VSPICS0 Not Output Enabled Matrix display, 0=enabled, 1=disabled
#define matrixDataPin 23      // VSPID Data In Matrix display

#define bigClockPin 14        // HSPICLK clock for the large 7-segment display
#define bigStrobePin 15       // HSPICS0 Strobe for the large 7-segment display 
#define bigDataPin 13         // HSPID data for the large 7-segment display

#define row1Pin 22
#define row2Pin 21
#define row3Pin 32
#define row4Pin 33
#define row5Pin 25
#define row6Pin 26
#define row7Pin 27

//#define numChar 19        // 1 displays of 19 characters
//#define numReg 12         // 12 8-bit shift registers total
#define numChar 57        // 3 displays of 19 characters each
#define numRow 7          // 7 rows of pixels per characters
#define numReg 36         // 36 8-bit shift registers total

#if CONFIG_FREERTOS_UNICORE
#define TASK_RUNNING_CORE 0
#else
#define TASK_RUNNING_CORE 1
#endif
